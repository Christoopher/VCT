#ifndef _OPENGL_VIEWER_
#define _OPENGL_VIEWER_

#ifndef GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
#define GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV             0x00000010
#endif

#include "LoadShaderUtility.h"
//C/C++ headers
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string>

//GlEW and GLFW
#include "glew.h"
#include "wglew.h"
#include "glfw.h"
#include "glut.h"

//GLTools
#include "math3d.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include "Shader.h"
#include "loadobj.h"
#include "CPUOctree.h"
#include "ObjModel.h"
//----------------------------------------------------------------------------//
// Variables declaration
//----------------------------------------------------------------------------//


#include <iostream>
#include <bitset>
void float_bin(float f)
{
	int x = *(int *)&f;
	std::bitset<sizeof(int) * 8> binary(x);
	std::cout << binary << std::endl;    
}

void int_bin(int i)
{
	int x = *(int *)&i;
	std::bitset<sizeof(int) * 8> binary(x);
	std::cout << binary << std::endl;    
}

int floatBitsToInt(float const & value)
{
	union
	{
		float f;
		int i;
	} fi;

	fi.f = value;
	return fi.i;
}
#define LOW10 0x3FF
#define MID10 0xFFC00
#define HIGH10 0x3FF00000
Vec3f
unpack(float val)
{
	int ival = floatBitsToInt(val);
	return Vec3f( (ival & HIGH10) >> 20, (ival & MID10) >> 10, ival & LOW10);
}


std::string path;


//Window properties and GUI-related
int winw, winh;
int lastmousex, lastmousey, lastwheelpos;
float posDx = 0.0f, posDy = 0.0f, zoom = 0.0f, rotDx = 0.0f, rotDy = 0.0f;
double t0 = 0;
int frames = 0;
bool running = true;

bool left = false, right = false, forward = false, back = false;

Model * model;
Model * modelPhong;
ObjModel objModel;

GLuint VBO;

//Wireframe vbos
GLuint wf_vbo, wf_scale;

Shader shader;
Shader fragListShader;
Shader voxelShader;
Shader phongShader;

//OpenGLTools
GLFrame cameraFrame;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;


//Atomic counter buffer
GLuint acBuffer;
//xyz buffer
GLuint xyzBuffer; //gpu
std::vector<Vec3f> h_xyzBuffer; //cpu
int XYZ_VALUES = 70000000*3;
int XYZ_BUFFER_SIZE = XYZ_VALUES*sizeof(float);
//xyzBuffer global memory addresses
GLuint64EXT xyzBufferGPUAddress=0;

CPUOctree octree;

bool changed = true;
int currLevel = 2;

//Full grid Tex3D
GLuint volumeTexture;

//Scene bounding volume
float MINX = -10, MAXX = 10, MINY = -10, MAXY = 10, MINZ = -10, MAXZ = 10;
int GRIDDIM = 512;

GLfloat * buffer;

//Full screen quad
GLfloat quadVerts[] = {	-1.0f, -1.0f, 0.0f,
						-1.0f,  1.0f, 0.0f,
						1.0f,  1.0f, 0.0f,
						1.0f, -1.0f, 0.0f};

GLfloat quadTex[] = {
						0.0f, 0.0f,
						0.0f, 1.0f,
						1.0f, 1.0f,
						1.0f, 0.0f};

GLuint quadIndices[] = {0,2,1,0,3,2};


GLfloat	quadUnitCubeVertices[] =
{ 
	1.0f,1.0f,1.0f,1.0f,  0.0f,1.0f,1.0f,1.0f,  0.0f,0.0f,1.0f,1.0f,  1.0f,0.0f,1.0f,1.0f,
	1.0f,0.0f,0.0f,1.0f,  1.0f,1.0f,0.0f,1.0f,	0.0f,1.0f,0.0f,1.0f,  0.0f,0.0f,0.0f,1.0f
};

// The associated indices.
GLuint quadUnitCubeIndices[] = { 
//front face
	0,1,2,3,
//right face
	0,3,4,5,
//left face
	1,6,7,2,
//bottom face
	2,7,4,3,
//up face
	0,5,6,1,
//back face
	4,7,6,5
};


//----------------------------------------------------------------------------//
// Unit wireframe cube: vertices,normals & indices
//----------------------------------------------------------------------------//
GLfloat wireframeUnitCubeVertices[] = 
{ 
	-0.5,0.5,-0.5,  -0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,0.5,-0.5f,
	-0.5,-0.5,-0.5,  -0.5,-0.5,0.5,	0.5,-0.5,0.5, 0.5,-0.5,-0.5
};


//GLuint wireframeUnitcubeindices[] = { 0,1, 1,2, 2,3, 0,3, 0,4, 4,7, 7,3, 6,7, 2,6, 5,6, 1,5, 4,5 };

GLuint wireframeUnitcubeindices[] = { 0,1,2,3,4,5,6,7 };



//----------------------------------------------------------------------------//
// Print opengl error to console
//----------------------------------------------------------------------------//
void getOpenGLError() 
{
	GLenum errCode; 
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error: %s\n", errString);
		//std::cin.get(); //User must see the error
	}
	
}


float getFPS(void)
{
	double t = glfwGetTime(); 
	return (float)((double)frames / (t - t0));

}
//----------------------------------------------------------------------------//
// Display fps, winh, winw, zoom and title
//----------------------------------------------------------------------------//
void showFPS(int winw, int winh, float zoom) {

	static char titlestr[200];
	double t, fps;
	// Get current time
	t = glfwGetTime();  // Number of seconds since glfwInit()
	// If one second has passed, or if this is the very first frame
	if( (t - t0) > 1.0 || frames == 0 )
	{
		fps = (double)frames / (t - t0);
		sprintf(titlestr, "FLIP3D, %dx%d pixels, %.1fx zoom, %.1f FPS -> %.1f Mpixels/s",
			winw, winh, zoom, fps, winw*winh*fps*1e-6);
		glfwSetWindowTitle(titlestr);
		t0 = t;
		frames = 0;
	}
	frames ++;
}




//----------------------------------------------------------------------------//
// Cleans up the OpenGL viewer: Destroy window, destroys shaders, fbos, vbos, etc...
//----------------------------------------------------------------------------//
void TerminateViewer()
{
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&xyzBuffer);
	glDeleteBuffers(1,&acBuffer);

	//Delete texture
	glDeleteTextures(1,&volumeTexture);

	delete buffer;

	//Terminate the window
	glfwTerminate();
}

//----------------------------------------------------------------------------//
// Responsible for updating OpenGL due to screen resizing
//----------------------------------------------------------------------------//
void Resize()
{
	glfwGetWindowSize(&winw, &winh);
	glViewport(0, 0, winw, winh);


	// Calculate the projection matrix and bind it to shader
	viewFrustum.SetPerspective(45.0f, (GLfloat) winw / (GLfloat) winh, 1.0f, 10000000.0f);	
	//viewFrustum.SetOrthographic(MINX,MAXX,MINY,MAXY,MINZ,MAXZ);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}


void
resetCounter()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, acBuffer);
	GLuint * acValue = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
												 GL_MAP_WRITE_BIT);
	std::cout << "acCount: " << acValue[0] << "\n";	
	acValue[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}


void
readFragmentList()
{

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, acBuffer);
	GLuint * acValue = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
												 GL_MAP_READ_BIT);
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	//Read fragmentlist
	glBindBuffer(GL_ARRAY_BUFFER, xyzBuffer);
	GLfloat * d_xyzBufferPtr = (GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER, 0, XYZ_BUFFER_SIZE,
												GL_MAP_READ_BIT);
	getOpenGLError();											
	
	h_xyzBuffer.resize(*acValue);
	memcpy(&h_xyzBuffer[0], d_xyzBufferPtr,*acValue*3*sizeof(float));
	
	getOpenGLError();
	
	for(int i = 0;i<10;++i)
	{
		std::cout << unpack(h_xyzBuffer[i](0)) << ", ";
	}
	std::cout << std::endl;	

	bool ok = glUnmapBuffer(GL_ARRAY_BUFFER);
	if(!ok)
		std::cout << "corrupt data\n";
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	getOpenGLError();

}


void
voxelize()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0,0,GRIDDIM, GRIDDIM);

	fragListShader.use();
	getOpenGLError();
	
	resetCounter();

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D,volumeTexture);
	//glBindImageTextureEXT(0, volumeTexture, 0, GL_TRUE, 0,  GL_READ_WRITE, GL_RGBA32F);
	//glUniform1i(glGetUniformLocation(fragListShader(),"gridTex"),0);	

	GLfloat ortho[16] =  { 1,0,0,0,  0,1,0,0, 0,0,1,0, 0,0,0,1 };

	ortho[0] = 2.0f / (MAXX - MINX);
	ortho[5] = 2.0f / (MAXY - MINY);
	ortho[10] = -2.0f / (MAXZ - MINZ);
	ortho[12] = -((MAXX + MINX)/(MAXX - MINX));
	ortho[13] = -((MAXY + MINY)/(MAXY - MINY));
	ortho[14] = -((MAXZ + MINZ)/(MAXZ - MINZ));
	ortho[15] = 1.0f;

	modelViewMatrix.PushMatrix();
	modelViewMatrix.LoadIdentity();
	modelViewMatrix.Scale(0.005,0.005,0.005);
	glUniformMatrix4fv(fragListShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix4fv(fragListShader.getUniform("projectionMatrix"), 1, GL_FALSE, ortho);	

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,0,acBuffer);
	getOpenGLError();
	
	glDisable(GL_CULL_FACE);	
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	DrawModel(model);

	modelViewMatrix.PopMatrix();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glEnable(GL_CULL_FACE);	
	glEnable(GL_DEPTH_TEST);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,0,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	readFragmentList();
		
	//Remember to reset viewport
	glViewport(0,0,winw, winh);
}

void
drawQuad()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	
	glVertexAttribPointer(shader.getAttrib("vertex"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.getAttrib("vertex"));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,volumeTexture);
	glBindImageTexture(0, volumeTexture, 0, GL_TRUE, 0,  GL_READ_WRITE, GL_RGBA32F);
	glUniform1i(glGetUniformLocation(shader(),"gridTex"),0);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.LoadIdentity();
	projectionMatrix.PushMatrix();
	projectionMatrix.LoadIdentity();

	glUniformMatrix4fv(shader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix() );
	glUniformMatrix4fv(shader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetProjectionMatrix() );		

	glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, quadIndices);

	modelViewMatrix.PopMatrix();	
	projectionMatrix.PopMatrix();

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
}

void
drawOctree()
{
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Voxels & voxels= octree.getVoxels();
	
	voxelShader.use();

	
	
	//instanced positions


	glBindBuffer(GL_ARRAY_BUFFER,wf_vbo);
	//glBufferData(GL_ARRAY_BUFFER,voxels.pos.size()*3*sizeof(float),&voxels.pos[0],GL_STATIC_DRAW);
	glEnableVertexAttribArray(voxelShader.getAttrib("vertex"));
	glVertexAttribPointer(voxelShader.getAttrib("vertex"),3,GL_FLOAT,GL_FALSE,0,0);	

	glUniformMatrix4fv(voxelShader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetProjectionMatrix() );
	glUniformMatrix4fv(voxelShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix() );
	int connectivity[16] = {5,1,2,6, 1,0,3,2, 0,4,7,3, 4,5,6,7};
	glUniform4iv(voxelShader.getUniform("connectivity"), 4, connectivity );
	glUniform1f(voxelShader.getUniform("scale"),voxels.scale);
	GLfloat corners[] = 
	{ 
		-0.5,0.5,0.5, -0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5,
		-0.5,0.5,-0.5, -0.5,-0.5,-0.5, 0.5,-0.5,-0.5, 0.5,0.5,-0.5
	};
	glUniform3fv(voxelShader.getUniform("corners"), 8, corners );
	getOpenGLError();
	//glDrawElements(GL_POINTS, 8, GL_UNSIGNED_INT,0);
	glDrawArrays(GL_POINTS,0,voxels.pos.size());

	/*
	for(int i = 0; i < voxel.size();++i) {
		modelViewMatrix.PushMatrix();
		float s = voxel[i].scale*0.5;
		modelViewMatrix.Translate(voxel[i].pos(0)-10+s,voxel[i].pos(1)-10+s,voxel[i].pos(2)-10+s);
		glUniformMatrix4fv(voxelShader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetProjectionMatrix() );
		glUniformMatrix4fv(voxelShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix() );
		

		glutWireCube(voxel[i].scale);

		modelViewMatrix.PopMatrix();
		
	}*/
	
}

//----------------------------------------------------------------------------//
// Draws all content
//----------------------------------------------------------------------------//
bool first = true;
void OpenGl_drawAndUpdate(bool &running)
{
	//Disable vsync
	//wglSwapIntervalEXT(0);
	
	getOpenGLError();
	running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

	if(!running)
		return;

	showFPS(winw, winh, zoom);
	Resize(); //Update viewport and projection matrix
	
	/*if(forward) {
		cameraFrame.MoveForward(0.5);
	}
	if(back) {
		cameraFrame.MoveForward(-0.5);
	}
	if(left) {
		cameraFrame.MoveRight(0.5);
	}
	if(right) {
		cameraFrame.MoveRight(-0.5);
	}
	*/

	//Camera Matrix
	/*
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);		
	modelViewMatrix.PushMatrix(mCamera);
	float x = cameraFrame.GetOriginX();
	float y = cameraFrame.GetOriginY();
	float z = cameraFrame.GetOriginZ();
	modelViewMatrix.Translate(x,y,z);	
	modelViewMatrix.Rotate(-rotDx,1.0f,0.0f,0.0f);
	modelViewMatrix.Rotate(-rotDy,0.0f,1.0f,0.0f);
	*/

	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);		
	modelViewMatrix.PushMatrix(mCamera);
	modelViewMatrix.Translate(posDx,posDy,zoom);
	modelViewMatrix.Rotate(-rotDx,1.0f,0.0f,0.0f);
	modelViewMatrix.Rotate(-rotDy,0.0f,1.0f,0.0f);	
	

	//Clear the buffer color and depth
	
	
	
	if(first) {
		
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, GRIDDIM,GRIDDIM,GRIDDIM,0, GL_RGBA, GL_FLOAT,buffer);
		voxelize();
		first = false;
		glMemoryBarrierEXT(GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV);

		//while(h_xyzBuffer.size() > 0) {
		//	fragBuffer.push_back(h_xyzBuffer.back());
		//	h_xyzBuffer.pop_back();
		//}
		//buffer.insert(buffer.begin(), h_xyzBuffer.begin(),h_xyzBuffer.end());

		float max = -100000;
		for(int i = 0; i < h_xyzBuffer.size(); ++i) {
			if(h_xyzBuffer[i](0) > max)
				max = h_xyzBuffer[i](0);
		}
		octree.buildTree(9,h_xyzBuffer);
		h_xyzBuffer.clear();
		first = false;
	}

	if(changed) {
		octree.buildVoxel(currLevel,20);	

		glBindBuffer(GL_ARRAY_BUFFER,wf_vbo);
		glBufferData(GL_ARRAY_BUFFER,octree.getVoxels().pos.size()*3*sizeof(float),&octree.getVoxels().pos[0],GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		changed = false;
	}
	
	//drawQuad();
	
	
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw full screen quad ad ray trace the volume
	
	glEnable(GL_CULL_FACE);	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);	


	
	drawOctree();
	
	
	phongShader.use();
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.005,0.005,0.005);
	glUniformMatrix4fv(phongShader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix() );
	glUniformMatrix4fv(phongShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix() );
	glUniformMatrix3fv(phongShader.getUniform("normalMatrix"), 1, GL_FALSE, transformPipeline.GetNormalMatrix() );
	
	DrawModelProgram(model, phongShader(), "vertex", "normal");
	
	modelViewMatrix.PopMatrix();
	
	/*
	
	phongShader.use();
	glUniformMatrix4fv(phongShader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix() );
	glUniformMatrix4fv(phongShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix() );
	glUniformMatrix3fv(phongShader.getUniform("normalMatrix"), 1, GL_FALSE, transformPipeline.GetNormalMatrix() );
	objModel.drawModel(phongShader(),"vertex", "normal");
	*/
	
	glfwSwapBuffers();
	//Pop camera matrix
	modelViewMatrix.PopMatrix();
	glGetError();
}



void
initShader()
{	

	phongShader.addShader(path+"../../src/Shaders/phongShader.vert",Shader::VERTEX_SHADER);
	phongShader.addShader(path+"../../src/Shaders/phongShader.frag",Shader::FRAGMENT_SHADER);
	phongShader.addUniform("modelViewMatrix");
	phongShader.addUniform("projectionMatrix");
	phongShader.addUniform("normalMatrix");
	phongShader.addAttribute("vertex");
	phongShader.addAttribute("normal");
	phongShader.addAttribute("texCoords");
	phongShader.compile();
	getOpenGLError();

	voxelShader.addShader(path+"../../src/Shaders/voxelShader.vert",Shader::VERTEX_SHADER);
	voxelShader.addShader(path+"../../src/Shaders/voxelShader.frag", Shader::FRAGMENT_SHADER);
	voxelShader.addShader(path+"../../src/Shaders/voxelShader.geom", Shader::GEOMETRY_SHADER);
	voxelShader.addUniform("modelViewMatrix");
	voxelShader.addUniform("projectionMatrix");
	voxelShader.addUniform("connectivity");
	voxelShader.addUniform("corners");
	voxelShader.addUniform("scale");
	voxelShader.addAttribute("vertex");
	voxelShader.compile();
	getOpenGLError();

	shader.addShader(path+"../../src/Shaders/shader.vert", Shader::VERTEX_SHADER);
	shader.addShader(path+"../../src/Shaders/shader.frag", Shader::FRAGMENT_SHADER);
	shader.addAttribute("vertex");
	shader.addUniform("modelViewMatrix");
	shader.addUniform("projectionMatrix");
	shader.addUniform("gridTex");
	shader.compile();
	getOpenGLError();
	
	fragListShader.addShader(path+"../../src/Shaders/buildFragList.vert", Shader::VERTEX_SHADER);
	fragListShader.addShader(path+"../../src/Shaders/buildFragList.frag", Shader::FRAGMENT_SHADER);
	fragListShader.addShader(path+"../../src/Shaders/buildFragList.geom", Shader::GEOMETRY_SHADER);
	fragListShader.addAttribute("vertex");
	fragListShader.addAttribute("normal");
	fragListShader.addAttribute("texCoords");
	fragListShader.addUniform("modelViewMatrix");
	fragListShader.addUniform("projectionMatrix");
	fragListShader.addUniform("d_xyzBuffer");
	//fragListShader.addUniform("boxMin");
	//fragListShader.addUniform("boxMax");
	fragListShader.addUniform("gridDim");
	fragListShader.addUniform("gridTex");
	fragListShader.compile();
	getOpenGLError();

	glUseProgram(fragListShader());
	//glUniform3f(fragListShader.getUniform("boxMin"),MINX,MINY,MINZ);
	//glUniform3f(fragListShader.getUniform("boxMax"),MAXX,MAXY,MAXZ);	
	glUniform1i(fragListShader.getUniform("gridDim"),GRIDDIM);		
	glProgramUniform1i(fragListShader(), glGetUniformLocation(fragListShader(), "gridTex"), 0);
	glUseProgram(0);
	getOpenGLError();
}

void initTextures()
{
	glEnable(GL_TEXTURE_3D);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volumeTexture);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);	
	glBindImageTexture(0, volumeTexture, 0, GL_TRUE, 0,  GL_READ_WRITE, GL_RGBA32F);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	
	getOpenGLError();
	
	buffer = new GLfloat[GRIDDIM*GRIDDIM*GRIDDIM*4];
	int center = 256;
	for(int k = 0; k < GRIDDIM; ++k) {
		for(int j = 0; j < GRIDDIM; ++j) {
			for(int i = 0; i < GRIDDIM; ++i) {
				int offset = i + GRIDDIM*(j + GRIDDIM*k);

				int ic = center - i;
				int jc = center - j;
				int kc = center - k;
				float dist = sqrt(float(ic*ic + jc*jc + kc*kc));

				float val = 0;
				//if(dist < 50) {
				//	val = 1;
				//}

				buffer[offset*4] = val;
				buffer[offset*4+1] = val;
				buffer[offset*4+2] = val;
				buffer[offset*4+3] = val;
			}
		}
	}

	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, GRIDDIM,GRIDDIM,GRIDDIM,0, GL_RGBA, GL_FLOAT,buffer);
	
	
	
	glBindTexture(GL_TEXTURE_3D, 0);
	getOpenGLError();

}

void
initBuffers()
{
	//Atomic Counter Buffer
	glGenBuffers(1,&acBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,acBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,0);

	//Fullscreen quad buffer	
	glGenBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quadVerts),quadVerts,GL_STATIC_DRAW);	
	glVertexAttribPointer(shader.getAttrib("vertex"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.getAttrib("vertex"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Voxel fragment buffers	
	glGenBuffers(1, &xyzBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xyzBuffer);
	glBufferData(GL_ARRAY_BUFFER, XYZ_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);
	glMakeBufferResidentNV(GL_ARRAY_BUFFER, GL_READ_WRITE);	
	
	glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &xyzBufferGPUAddress); 
	glProgramUniformui64NV(fragListShader(), fragListShader.getUniform("d_xyzBuffer"), xyzBufferGPUAddress);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	getOpenGLError();	

	//Wireframe box vbo
	glGenBuffers(1,&wf_vbo);
	glGenBuffers(1,&wf_scale);
	
	
	//glVertexAttribPointer(wf_vertexLocation,4,GL_FLOAT,GL_FALSE,0,0);
	//glEnableVertexAttribArray(wf_vertexLocation);

	//Load model
//C:\Users\Teneo\Documents\Visual Studio 2010\Projects\VCT\models\teapot.obj
	//model = LoadModel("..\\..\\models\\teapot.obj");	
	std::string modelPath = path+"..\\..\\models\\blender.obj";
	model = LoadModel(const_cast<char *>(modelPath.c_str()));
	//model = LoadModel("..\\..\\models\\head\\head.obj");	
	//model = LoadModel("..\\..\\models\\dabsponza\\sponza.obj");	
	BuildModelVAO(model, fragListShader(), "vertex", "normal", "texCoords");

	//modelPhong = LoadModel("..\\..\\models\\head\\head.obj");
	//modelPhong = LoadModel("..\\..\\models\\blender.obj");
	//modelPhong = LoadModel("..\\..\\models\\teapot.obj");
	//modelPhong = LoadModel("..\\..\\models\\dabsponza\\sponza.obj");	
	//BuildModelVAO(modelPhong, phongShader(), "vertex", "normal", "texCoords");
	

	//objModel.loadModel("..\\..\\models\\teapot.obj");
	//objModel.loadModel("..\\..\\models\\head\\head.obj");

	getOpenGLError();
}


//----------------------------------------------------------------------------//
// GLFW Keyboard callback
//----------------------------------------------------------------------------//
void GLFWCALL KeyboardFunc( int key, int action )
{

	if(key == 'K' && action == GLFW_PRESS) //Cam reset
	{
		if(currLevel > 1)
			--currLevel;
		changed = true;
	}

	if(key == 'L' && action == GLFW_PRESS) //Cam reset
	{
		if(currLevel<9)
		++currLevel;
		changed = true;
	}


	if(key == 'C' && action == GLFW_PRESS) //Cam reset
	{
		rotDy = 0;
		rotDx = 0;
		posDx = 0;
		posDy = 0;
		zoom = 0;
		cameraFrame.SetOrigin(0.0f,0.0f,5.0);
		cameraFrame.SetForwardVector(0,0,-1);

	}

	if(key == 'W')
		if(action == GLFW_PRESS)
			forward = true;
		else
			forward = false;

	if(key == 'A')
		if(action == GLFW_PRESS)
			left = true;
		else
			left = false;

	if(key == 'S')
		if(action == GLFW_PRESS)
			back = true;
		else
			back = false;

	if(key == 'D')
		if(action == GLFW_PRESS)
			right = true;
		else
			right = false;

	if(key == GLFW_KEY_ESC)
	{
		running = false;
	}
}


//----------------------------------------------------------------------------//
// GLFW MouseButton callback
//----------------------------------------------------------------------------//
void GLFWCALL MouseButtonFunc( int button, int action )
{

}

//----------------------------------------------------------------------------//
// GLFW MouseWheel callback
//----------------------------------------------------------------------------//
void GLFWCALL MouseWheelFunc( int pos )
{
	zoom += (pos - lastwheelpos) *1.2;

	lastwheelpos = pos;
}

//----------------------------------------------------------------------------//
// GLFW MouseButton callback
//----------------------------------------------------------------------------//
void GLFWCALL MousePosFunc( int x, int y )
{
	if(glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS && glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		float rDx = (lastmousex - x) * 0.5;
		float rDy = (lastmousey - y) * 0.5;
		rotDy += rDx;
		rotDx += rDy;
	}
	else if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		float dx = (lastmousex - x) * 0.05;
		float dy = (lastmousey - y) * 0.05;
		posDx -= dx;
		posDy += dy;

		//cameraFrame.MoveRight(dx);
		//cameraFrame.MoveUp(dy);
	}
	lastmousex = x;
	lastmousey = y;
}


//----------------------------------------------------------------------------//
// Creates and sets up a window
//----------------------------------------------------------------------------//
void OpenGl_initViewer(int width_, int height_)
{
	winw = width_;
	winh = height_;

	// Initialize GLFW
	if( !glfwInit() )
	{
		exit( EXIT_FAILURE );
	}

	

	// Open an OpenGL window using glfw
	if( !glfwOpenWindow( winw,winh, 8,8,8,8,32,0, GLFW_WINDOW ) )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	//Init glew!
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//Setup callbacks
	glfwSetKeyCallback(KeyboardFunc);
	glfwSetMouseButtonCallback(MouseButtonFunc);
	glfwSetMousePosCallback(MousePosFunc);
	glfwSetMouseWheelCallback(MouseWheelFunc);

	glfwGetMousePos(&lastmousex, &lastmousey);
	lastwheelpos = glfwGetMouseWheel();

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glShadeModel(GL_SMOOTH);

	//Move the camera back 5 units
	cameraFrame.SetOrigin(0.0f,0.0f,5.0);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);

	TCHAR buffer[MAX_PATH];
	GetModuleFileName( NULL, buffer, MAX_PATH );
	GetCurrentDirectory(MAX_PATH,buffer);
	std::wstring arr_w( buffer );
	std::string arr_s( arr_w.begin(), arr_w.end() );
	path = arr_s;
	path += "\\";
	//std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
	//std::string path = std::string( buffer ).substr( 0, pos);
	
	
	initShader();

	//Must be done after shader init
	initBuffers();

	//initTextures();
}


#endif