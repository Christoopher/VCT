
#include <ObjLoader.h>
#include <set>

/* Call this function to load a model, only loads triangulated meshes */
void LoadObjMesh(std::string filename, ObjMesh & mesh) {

	/*std::vector<Vector3f>           positions;
	std::vector<Vector2f>           texcoords;
	std::vector<Vector3f>           normals;
	std::vector<_ObjMeshFaceIndex>  faces;
	*/
	/**
	 * Load file, parse it
	 * Lines beginning with: 
	 * '#'  are comments can be ignored
	 * 'v'  are vertices positions (3 floats that can be positive or negative)
	 * 'vt' are vertices texcoords (2 floats that can be positive or negative)
	 * 'vn' are vertices normals   (3 floats that can be positive or negative)
	 * 'f'  are faces, 3 values that contain 3 values which are separated by / and <space>
	 */

	std::vector<int> posIndices;
	std::vector<int> normalIndices;
	std::vector<int> texIndices;
	std::vector<int> faceIndices;
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texCoords;

	std::ifstream filestream;
	filestream.open(filename.c_str());

	std::string line_stream;	// No longer depending on char arrays thanks to: Dale Weiler
	while(std::getline(filestream, line_stream)){	
		std::stringstream str_stream(line_stream);
		
		std::string type_str;
		str_stream >> type_str;
		if(type_str == TOKEN_VERTEX_POS){
			Vector3f pos;
			str_stream >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos.x);
			positions.push_back(pos.y);
			positions.push_back(pos.z);
		}else if(type_str == TOKEN_VERTEX_TEX){
			Vector2f tex;
			str_stream >> tex.x >> tex.y;
			texCoords.push_back(tex.x);
			texCoords.push_back(tex.y);
		}else if(type_str == TOKEN_VERTEX_NOR){
			Vector3f nor;
			str_stream >> nor.x >> nor.y >> nor.z;
			normals.push_back(nor.x);
			normals.push_back(nor.y);
			normals.push_back(nor.z);
		}else if(type_str == TOKEN_FACE){
			//_ObjMeshFaceIndex face_index;
			char interupt;

			for(int i = 0; i < 3; ++i){
				int first,sec,third;
			
				str_stream >> first >> interupt 
						   >> sec >> interupt 
						   >> third;
				
				faceIndices.push_back(first);
				texIndices.push_back(sec);
				normalIndices.push_back(third);
			}
			
		}
	}
	// Explicit closing of the file 
	filestream.close();

	std::set<int> idx;
	
	

	for(int i = 0; i < faceIndices.size(); ++i) {
		mesh.indices.push_back(faceIndices[i]-1);
		if(idx.count(faceIndices[i]-1) > 0)
			continue;

		idx.insert(faceIndices[i]-1);

		mesh.positions.push_back(positions[(faceIndices[i]-1)*3]);
		mesh.positions.push_back(positions[(faceIndices[i]-1)*3+1]);
		mesh.positions.push_back(positions[(faceIndices[i]-1)*3+2]);

		mesh.normals.push_back(normals[(normalIndices[i]-1)*3]);
		mesh.normals.push_back(normals[(normalIndices[i]-1)*3+1]);
		mesh.normals.push_back(normals[(normalIndices[i]-1)*3+2]);

		mesh.texcoords.push_back(texCoords[(texIndices[i]-1)*2]);
		mesh.texcoords.push_back(texCoords[(texIndices[i]-1)*2+1]);
	}

	/*
	idx.clear();

	for(int i = 0; i < normalIndices.size(); ++i) {

		if(idx.count(normalIndices[i]-1) > 0)
			continue;
		idx.insert(normalIndices[i]-1);

		mesh.normals.push_back(normals[(normalIndices[i]-1)*3]);
		mesh.normals.push_back(normals[(normalIndices[i]-1)*3+1]);
		mesh.normals.push_back(normals[(normalIndices[i]-1)*3+2]);
	}

	idx.clear();

	for(int i = 0; i < texIndices.size(); ++i) {

		if(idx.count(texIndices[i]-1) > 0)
			continue;
		idx.insert(texIndices[i]-1);

		mesh.texcoords.push_back(texCoords[(texIndices[i]-1)*2]);
		mesh.texcoords.push_back(texCoords[(texIndices[i]-1)*2+1]);
	}


	

	/*
	for(size_t i = 0; i < faces.size(); ++i){	
		ObjMeshFace face;
		for(size_t j = 0; j < 3; ++j){
			face.vertices[j].pos        = positions[faces[i].pos_index[j] - 1];
			face.vertices[j].texcoord   = texcoords[faces[i].tex_index[j] - 1];
			face.vertices[j].normal     = normals[faces[i].nor_index[j] - 1];
		}
		myMesh.faces.push_back(face);
	}
	*/
}