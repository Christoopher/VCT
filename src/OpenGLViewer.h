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
//----------------------------------------------------------------------------//
// Variables declaration
//----------------------------------------------------------------------------//

//Window properties and GUI-related
int winw, winh;
int lastmousex, lastmousey, lastwheelpos;
float posDx = 0.0f, posDy = 0.0f, zoom = 0.0f, rotDx = 0.0f, rotDy = 0.0f;
double t0 = 0;
int frames = 0;
bool running = true;

Model * model;

GLuint VBO;

Shader shader;
Shader fragListShader;

//OpenGLTools
GLFrame cameraFrame;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;


//Atomic counter buffer
GLuint acBuffer;
//xyz buffer
GLuint xyzBuffer;
//int XYZ_BUFFER_SIZE = 600*600*sizeof(float)*3;
int XYZ_BUFFER_SIZE = 1000000*sizeof(float)*3;
//xyzBuffer global memory addresses
GLuint64EXT xyzBufferGPUAddress=0;

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
// Print opengl error to console
//----------------------------------------------------------------------------//
void getOpenGLError() 
{
	GLenum errCode; 
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error: %s\n", errString);
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
	//viewFrustum.SetPerspective(45.0f, (GLfloat) winw / (GLfloat) winh, 1.0f, 10000000.0f);	
	viewFrustum.SetOrthographic(-10,10,-10,10,-10,10);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}

//----------------------------------------------------------------------------//
// Draws all content
//----------------------------------------------------------------------------//

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


	//Camera Matrix
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);		
	modelViewMatrix.PushMatrix(mCamera);
	modelViewMatrix.Translate(posDx,posDy,zoom);
	modelViewMatrix.Rotate(-rotDx,1.0f,0.0f,0.0f);
	modelViewMatrix.Rotate(-rotDy,0.0f,1.0f,0.0f);
	


	//Clear the buffer color and depth
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//glUseProgram(shader);	
	fragListShader.use();
	getOpenGLError();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	

	//modelViewMatrix.LoadIdentity();
	//projectionMatrix.LoadIdentity();
	

	glUniformMatrix4fv(fragListShader.getUniform("modelViewMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix4fv(fragListShader.getUniform("projectionMatrix"), 1, GL_FALSE, transformPipeline.GetProjectionMatrix());	

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,0,acBuffer);
	getOpenGLError();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	DrawModel(model);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, quadIndices);
	glFlush();

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,0,0);

	modelViewMatrix.PopMatrix();
	projectionMatrix.PopMatrix();
	glBindBuffer(GL_ARRAY_BUFFER, 0);		
	modelViewMatrix.PopMatrix();
	//glFlush();
	
	
	//Read the counter;
	/*
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, acBuffer);
	
	GLuint * acValue = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
												 GL_MAP_READ_BIT);
	getOpenGLError();
	std::cout << "acCount: " << *acValue << "\n";	
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	*/
	

	//Set counter to zero
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, acBuffer);
	GLuint * acValue = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
												 GL_MAP_WRITE_BIT);
	std::cout << "acCount: " << acValue[0] << "\n";	
	acValue[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	
	//Read fragmentlist	
	/*
	int size = 600*600;
	glBindBuffer(GL_ARRAY_BUFFER, xyzBuffer);
	glFlush();	
	GLfloat * d_xyzBufferPtr = (GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER, 0, XYZ_BUFFER_SIZE,
												GL_MAP_WRITE_BIT);
	getOpenGLError();											
	
	std::vector<GLfloat> h_xyzBuffer;
	h_xyzBuffer.resize(size*3);
	memcpy(&h_xyzBuffer[0], d_xyzBufferPtr,XYZ_BUFFER_SIZE);
	
	getOpenGLError();
	for(int i = 0;i<50;++i)
	{
		std::cout << "xyz : " << h_xyzBuffer[i] << ", " << h_xyzBuffer[i*3+1] << ", " << h_xyzBuffer[i*3+2] << " \n";
	}
	std::cout << std::endl;

	bool ok = glUnmapBuffer(GL_ARRAY_BUFFER);
	if(!ok)
		std::cout << "corrupt data\n";
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	getOpenGLError();
	*/
	

	glfwSwapBuffers();
	modelViewMatrix.PopMatrix();
	glGetError();
}



void
initShader()
{	
	shader.addShader("../../src/Shaders/shader.vert", Shader::VERTEX_SHADER);
	shader.addShader("../../src/Shaders/shader.frag", Shader::FRAGMENT_SHADER);
	shader.addAttribute("vertex");

	shader.addUniform("modelViewMatrix");
	shader.addUniform("projectionMatrix");
	shader.compile();
	
	fragListShader.addShader("../../src/Shaders/buildFragList.vert", Shader::VERTEX_SHADER);
	fragListShader.addShader("../../src/Shaders/buildFragList.frag", Shader::FRAGMENT_SHADER);
	fragListShader.addShader("../../src/Shaders/buildFragList.geom", Shader::GEOMETRY_SHADER);
	fragListShader.addAttribute("vertex");
	fragListShader.addAttribute("normal");
	fragListShader.addAttribute("texCoords");
	fragListShader.addUniform("modelViewMatrix");
	fragListShader.addUniform("projectionMatrix");
	fragListShader.addUniform("d_xyzBuffer");
	fragListShader.compile();
	

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

	//Load model
//C:\Users\Teneo\Documents\Visual Studio 2010\Projects\VCT\models\teapot.obj
	model = LoadModel("..\\..\\models\\teapot.obj");
	BuildModelVAO(model, fragListShader(), "vertex", "normal", "texCoords");
	getOpenGLError();
}


//----------------------------------------------------------------------------//
// GLFW Keyboard callback
//----------------------------------------------------------------------------//
void GLFWCALL KeyboardFunc( int key, int action )
{

	if(key == 'C' && action == GLFW_PRESS) //Cam reset
	{
		rotDy = 0;
		rotDx = 0;
		posDx = 0;
		posDy = 0;
		zoom = 0;

	}

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
		rotDy += (lastmousex - x) * 0.5;
		rotDx += (lastmousey - y) * 0.5;
	}
	else if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		posDx -= (lastmousex - x) * 0.05;
		posDy += (lastmousey - y) * 0.05;
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

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);

	//Move the camera back 5 units
	cameraFrame.SetOrigin(0.0f,0.0f,5.0);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);



	
	initShader();

	//Must be done after shader init
	initBuffers();
}


#endif