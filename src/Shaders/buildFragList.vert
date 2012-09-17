#version 420

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;


out vec3 g_xyz;
out vec3 g_normal;
out vec2 g_texCoord;

void main(void)
{	
	g_xyz =  vec3(modelViewMatrix * vec4(vertex, 1.0));
	g_normal = normal;
	g_texCoord = texCoord;
	//gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex, 1.0);
	gl_Position = vec4(vertex, 1.0);
}