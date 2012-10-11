#version 330

// Incoming per vertex... position and normal
in vec4 vertex;
in vec3 normal;
uniform mat4   projectionMatrix;
uniform mat4   modelViewMatrix;
uniform mat3   normalMatrix;
// Color to fragment program
out vec3 vVaryingNormal;
out vec3 vVaryingLightDir;
void main(void) 
{
	vec3 vLightPosition = vec3(0, 100.0f, 0.0f);
	// Get surface normal in eye coordinates
	vVaryingNormal = normalMatrix * normal;
	// Get vertex position in eye coordinates
	vec4 vPosition4 = modelViewMatrix * vertex;
	vec3 vPosition3 = vPosition4.xyz / vPosition4.w;
	// Get vector to light source
	vVaryingLightDir = normalize(vLightPosition - vPosition3);
	// Don't forget to transform the geometry!
	gl_Position = projectionMatrix * vertex;
}


