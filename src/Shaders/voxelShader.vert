#version 420

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 vertex;

void main(void)
{	
	gl_Position = vec4(vertex,1.0);
}