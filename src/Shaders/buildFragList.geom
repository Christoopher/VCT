//GEOMETRY SHADER
 #version 420
 #extension GL_ARB_geometry_shader4 : enable

 // A triangle with three points come in
 layout(triangles) in;

 // Out of three triangle points, we create a triangle strip with two triangles
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 g_xyz[3];
in vec3 g_normal[3];
in vec2 g_texCoord[3];

out vec3 f_xyz;
out vec3 f_normal;
out vec2 f_texCoord;
out vec3 f_color;

#define xMax 10
#define xMin -10
#define yMax 10
#define yMin -10
#define zMax 10
#define zMin -10

void main()
 {
	mat4 rot2X = mat4(
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f,1.0f,0.0f,0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f,0.0f, 0.0f, 1.0f
	);

	mat4 rot2Y = mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 
	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);


	
	vec3 normal = g_normal[0]+g_normal[1]+g_normal[2];
	normal *= 0.33333;	
	vec3 absNormal = abs(normal);	
	int axis = 0;
	float maxVal = absNormal.x;
	if(absNormal.y > maxVal) {
		maxVal = absNormal.y;
		axis = 1;
	}
	if(absNormal.z > maxVal) {		
		axis = 2;
	}
	
	//Defualt rot if we should project on Z-Axis   RED
	mat4 rot = mat4(1.0f,0.0f,0.0f,0.0f,
					0.0f,1.0f,0.0f,0.0f,
					0.0f,0.0f,1.0f,0.0f,
					0.0f,0.0f,0.0f,1.0f);
	f_color = vec3(1.0f,0.0f,0.0f);
	if(axis == 0) {
		rot = rot2X; //Project on X-Axis     GREEN
		f_color = vec3(0.0f,1.0f,0.0f);
	} else if(axis == 1) {
		f_color = vec3(0.0f,0.0f,1.0f);
		rot = rot2Y; //Project on Y-Axis     BLUE
	}

	int i;
	//Pass through the original vertex
   for(i=0; i<gl_VerticesIn; i++)
   {
		gl_Position = projectionMatrix * modelViewMatrix * rot * gl_PositionIn[i];

		f_xyz = g_xyz[i];
		f_normal = g_normal[i];
		f_texCoord = g_texCoord[i];
		EmitVertex();
	}
	
	EndPrimitive();
}

