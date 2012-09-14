// GL utilities, bare essentials
// By Ingemar Ragnemalm

// August 2012:
// FBO creation/usage routines.
// Geometry shader support synched with preliminary version.

//#define GL3_PROTOTYPES
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "GL_utilities.h"

// From ShaderLoader

char* readFile(char *file)
{
	FILE *fptr;
	long length;
	char *buf;

	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */
	
	return buf; /* Return the buffer */
}

// Infolog: Show result of shader compilation
void printShaderInfoLog(GLuint obj)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

// Compile a shader, return reference to it
GLuint compileShaders(const char *vs, const char *fs, const char *gs)
{
	GLuint v,f,g,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	glCompileShader(v);
	glCompileShader(f);
	if (gs != NULL)
	{
		g = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(g, 1, &gs, NULL);
		glCompileShader(g);
	}

	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);
	if (gs != NULL)
		glAttachShader(p,g);
	glLinkProgram(p);
	glUseProgram(p);
	
	printShaderInfoLog(v);
	printShaderInfoLog(f);
	if (gs != NULL)	printShaderInfoLog(g);
	printProgramInfoLog(p);
	
	return p;
}

GLuint loadShaders(const char *vertFileName, const char *fragFileName)
{
	return loadShadersG(vertFileName, fragFileName, NULL);
}

GLuint loadShadersG(const char *vertFileName, const char *fragFileName, const char *geomFileName)
// With geometry shader support
{
	char *vs, *fs, *gs;
	GLuint p = 0;
	
	vs = readFile((char *)vertFileName);
	fs = readFile((char *)fragFileName);
	gs = readFile((char *)geomFileName);
	if (vs==NULL)
		printf("Failed to read %s from disk.\n", vertFileName);
	if (fs==NULL)
		printf("Failed to read %s from disk.\n", fragFileName);
	if ((gs==NULL) && (geomFileName != NULL))
		printf("Failed to read %s from disk.\n", geomFileName);
	if ((vs!=NULL)&&(fs!=NULL))
		p = compileShaders(vs, fs, gs);
	if (vs != NULL) free(vs);
	if (fs != NULL) free(fs);
	if (gs != NULL) free(gs);
	return p;
}

/*
// Compile a shader, return reference to it
GLuint compileShadersOBSOLETE(const char *vs, const char *fs)
{
	GLuint v,f,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);
	glLinkProgram(p);
	glUseProgram(p);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printProgramInfoLog(p);

	return p;
}

GLuint loadShaders(const char *vertFileName, const char *fragFileName)
// TO DO: Add geometry shader support
{
	// const
	char *vs, *fs;
	GLuint p;

	vs = readFile((char *)vertFileName);
	fs = readFile((char *)fragFileName);
	if (vs==NULL)
	{
		printf("Loading shader %s failed.\n", vertFileName);
		return 0;
	}
	if (fs==NULL)
	{
		printf("Loading shader %s failed.\n", fragFileName);
		return 0;
	}

	p = compileShaders(vs, fs);
	if (vs != NULL) free(vs);
	if (fs != NULL) free(fs);
	return p;
}
*/

// End of ShaderLoader


void dumpInfo(void)
{
   printf ("Vendor: %s\n", glGetString (GL_VENDOR));
   printf ("Renderer: %s\n", glGetString (GL_RENDERER));
   printf ("Version: %s\n", glGetString (GL_VERSION));
   printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
   printError ("dumpInfo");
}

/* report GL errors, if any, to stderr */
void printError(const char *functionName)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR)
   {
	  fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
   }
}


// Keymap mini manager
// Important! Uses glutKeyboardFunc/glutKeyboardUpFunc so you can't use them
// elsewhere or they will conflict.

char keymap[256];

char keyIsDown(unsigned char c)
{
	return keymap[(unsigned int)c];
}

void keyUp(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 0;
}

void keyDown(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 1;
}

void initKeymapManager()
{
	int i;
	for (i = 0; i < 256; i++) keymap[i] = 0;

	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
}


// FBO

//----------------------------------FBO functions-----------------------------------
void CHECK_FRAMEBUFFER_STATUS()
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer not complete\n");
}

// create FBO
FBOstruct *initFBO(int width, int height, int int_method)
{
	FBOstruct *fbo = malloc(sizeof(FBOstruct));

	fbo->width = width;
	fbo->height = height;

	// create objects
	glGenFramebuffers(1, &fbo->fb); // frame buffer id
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	glGenTextures(1, &fbo->texid);
	printf("%i \n",fbo->texid);
	glBindTexture(GL_TEXTURE_2D, fbo->texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (int_method == 0)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texid, 0);

	// Renderbuffer
	// initialize depth renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rb);
	CHECK_FRAMEBUFFER_STATUS();

	printf("Framebuffer object %d\n", fbo->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}

// create FBO, optionally with depth
FBOstruct *initFBO2(int width, int height, int int_method, int create_depthimage)
{
    FBOstruct *fbo = malloc(sizeof(FBOstruct));

    fbo->width = width;
    fbo->height = height;

    // create objects
    glGenFramebuffers(1, &fbo->fb); // frame buffer id
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
    glGenTextures(1, &fbo->texid);
    printf("%i \n",fbo->texid);
    glBindTexture(GL_TEXTURE_2D, fbo->texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (int_method == 0)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texid, 0);
    if (create_depthimage!=0)
    {
      glGenTextures(1, &fbo->depth);
      glBindTexture(GL_TEXTURE_2D, fbo->depth);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0L);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depth, 0);	
      printf("depthtexture: %i\n",fbo->depth);
    }

    // Renderbuffer
    // initialize depth renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->rb);
    CHECK_FRAMEBUFFER_STATUS();

    printf("Framebuffer object %d\n", fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

static int lastw = 0;
static int lasth = 0;

// choose input (textures) and output (FBO)
void useFBO(FBOstruct *out, FBOstruct *in1, FBOstruct *in2)
{
	GLint curfbo;
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
	if (curfbo == 0)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		lastw = viewport[2] - viewport[0];
		lasth = viewport[3] - viewport[1];
	}
	
	if (out != 0L)
		glViewport(0, 0, out->width, out->height);
	else
		glViewport(0, 0, lastw, lasth);

	if (out != 0L)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, out->fb);
		glViewport(0, 0, out->width, out->height);
	}
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	if (in2 != 0L)
		glBindTexture(GL_TEXTURE_2D, in2->texid);
	else
		glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	if (in1 != 0L)
		glBindTexture(GL_TEXTURE_2D, in1->texid);
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}
