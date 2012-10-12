
#ifndef OBJMODEL_H
#define OBJMODEL_H


#include <string>
#include <iostream>

//Obj parser
#include <ObjLoader.h>

//GlEW and GLFW
#include "glew.h"
#include "wglew.h"
#include "glfw.h"
#include "glut.h"

class ObjModel
{
public:
	ObjModel() {}

	void loadModel(std::string fullPath);
	void drawModel(GLuint,char* = 0,char* = 0,char* = 0);

private:
	ObjMesh _mesh;

	GLfloat* _vertexArray;
	GLfloat* _normalArray;
	GLfloat* _texCoordArray;
	GLfloat* _colorArray; // Rarely used
	GLuint* _indexArray;
	
	// Space for saving VBO and VAO IDs
	GLuint vao; // VAO
	GLuint vb, ib, nb, tb; // VBOs

// --------------------------------------------------------

	void
	_buildBuffers();
};


#endif


