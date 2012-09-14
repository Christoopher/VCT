
#version 430 compatibility

in vec3 xyz;

//GPU pointer to xyz buffer
//uniform float * d_xyzBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint ac;
void main(void)
{

	uint counter = atomicCounterIncrement(ac);
	float r = (counter/600) / 600.f;
	gl_FragColor = vec4(r, 0, 0, 1);
}
