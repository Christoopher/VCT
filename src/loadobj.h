#ifndef loadobj_h
#define loadobj_h

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#include "glew.h"
#include "glut.h"


typedef struct
{
  GLfloat* vertexArray;
  GLfloat* normalArray;
  GLfloat* texCoordArray;
  GLfloat* colorArray; // Rarely used
  GLuint* indexArray;
  int numVertices;
  int numIndices;
  
  // Space for saving VBO and VAO IDs
  GLuint vao; // VAO
  GLuint vb, ib, nb, tb; // VBOs
} Model;

Model* LoadModel(char* name);

// NEW:

void DrawModel(Model *m);
void DrawWireframeModel(Model *m);
void DrawModelProgram(Model *m, 
		  GLuint program,
		  char* vertexVariableName,
		  char* normalVariableName = 0,
		  char* texCoordVariableName = 0);

Model* LoadModelPlus(char* name,
			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);
void EnableModelForShader(Model *m, GLuint program, // NOT TESTED
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);
Model* LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd,
			
			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);
void BuildModelVAO(Model *m,
			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);

void CenterModel(Model *m);

#ifdef __cplusplus
}
#endif

#endif
