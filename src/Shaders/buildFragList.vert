#version 420

in vec3 vertex;
in vec3 normal;
in vec2 texCoords;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;


out vec3 xyz;

void main(void)
{	
	xyz =  vec3(modelViewMatrix * vec4(vertex, 1.0));
	gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex, 1.0);
}