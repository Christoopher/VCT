

#include <ObjModel.h>



void ObjModel::loadModel(std::string fullPath)
{
	LoadObjMesh(fullPath,_mesh);	
	
	_buildBuffers();
	
}


void ObjModel::_buildBuffers()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vb);
	glGenBuffers(1, &ib);
	glGenBuffers(1, &nb);
	if (_texCoordArray != NULL)
		glGenBuffers(1, &tb);
	
	glBindVertexArray(vao);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, _mesh.positions.size()*sizeof(GLfloat),&_mesh.positions[0], GL_STATIC_DRAW);
	

	// VBO for normal data
	if (_normalArray != NULL) {
		glBindBuffer(GL_ARRAY_BUFFER, nb);
		glBufferData(GL_ARRAY_BUFFER, _mesh.normals.size()*sizeof(GLfloat),&_mesh.normals[0], GL_STATIC_DRAW);
	}
	
	
	// VBO for texture coordinate data
	if (_texCoordArray != NULL) {
		glBindBuffer(GL_ARRAY_BUFFER, tb);
		glBufferData(GL_ARRAY_BUFFER, _mesh.texcoords.size()*sizeof(GLfloat),&_mesh.texcoords[0], GL_STATIC_DRAW);		
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh.indices.size()*sizeof(GLfloat),&_mesh.indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void ObjModel::drawModel(GLuint program,
						char* vertexVariableName,
						char* normalVariableName,
						char* texCoordVariableName)
{
	glBindVertexArray(vao);	// Select VAO

	if(vertexVariableName != 0) {
		glVertexAttribPointer(glGetAttribLocation(program, vertexVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0); 
		glEnableVertexAttribArray(glGetAttribLocation(program, vertexVariableName));
	}

	if(normalVariableName != 0) {
		glVertexAttribPointer(glGetAttribLocation(program, normalVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, normalVariableName));
	}

	if(texCoordVariableName != 0) {
		glVertexAttribPointer(glGetAttribLocation(program, texCoordVariableName), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, texCoordVariableName));
	}

	
	glDrawElements(GL_TRIANGLES, _mesh.indices.size(), GL_UNSIGNED_INT, 0L);
	
	glBindVertexArray(0);	// DEEESelect VAO
	


}