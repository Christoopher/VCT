#version 430 compatibility

in vec3 vertex;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

out vec3 xyz;

void main(void)
{	
	vec3 xyz =  vec3(modelViewMatrix * vec4(vertex, 1.0));
	gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex, 1.0);
}