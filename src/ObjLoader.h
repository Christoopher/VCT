
/**
* The MIT License
* 
* Copyright (c) 2010 Wouter Lindenhof (http://limegarden.net)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#define TOKEN_VERTEX_POS "v"
#define TOKEN_VERTEX_NOR "vn"
#define TOKEN_VERTEX_TEX "vt"
#define TOKEN_FACE "f"

struct Vector2f{
	float x, y;
};
struct Vector3f{
	float x, y, z;
};

struct ObjMeshVertex{
	Vector3f pos;
	Vector2f texcoord;
	Vector3f normal;
};

/* This is a triangle, that we can render */
struct ObjMeshFace{
	ObjMeshVertex vertices[3];
};

/* This contains a list of triangles */
struct ObjMesh{
	//std::vector<ObjMeshFace> faces;

	std::vector<float>      positions;
	std::vector<float>      texcoords;
	std::vector<float>      normals;
	std::vector<int>		indices;
};

/* Internal structure */
struct _ObjMeshFaceIndex{
	int pos_index[3];
	int tex_index[3];
	int nor_index[3];
};

/* Call this function to load a model, only loads triangulated meshes */
void LoadObjMesh(std::string filename, ObjMesh & mesh);

#endif