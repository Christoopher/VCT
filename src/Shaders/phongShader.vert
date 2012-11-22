#version 420

// Incoming per vertex... position and normal
in vec3 vertex;
in vec3 normal;
uniform mat4   projectionMatrix;
uniform mat4   modelViewMatrix;
uniform mat3   normalMatrix;
// Color to fragment program
out vec3 vVaryingNormal;
out vec3 ambientNormal;
out vec3 vVaryingLightDir;
out vec3 vFragPos;
void main(void) 
{
	ambientNormal = normal;
	vec3 vLightPosition = vec3(20.0f, 70.0f, 50.0f);
	// Get surface normal in eye coordinates
	vVaryingNormal = normalMatrix * normal;
	// Get vertex position in eye coordinates
	vec4 vPosition4 = modelViewMatrix * vec4(vertex,1.0);
	vec3 vPosition3 = vPosition4.xyz / vPosition4.w;
	// Get vector to light source
	vVaryingLightDir = normalize(vLightPosition - vPosition3);
	// Don't forget to transform the geometry!
	gl_Position = projectionMatrix * vec4(vertex,1.0);
	vFragPos = vertex.xyz;
}


