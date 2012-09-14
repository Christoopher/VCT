
#version 420

#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_load : enable
//#extension GL_NV_shader_buffer_store : enable


//GPU pointer to xyz buffer
uniform float *d_xyzBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint ac;

in vec3 xyz;
void main(void)
{
	//Counter must be int to work properly when indexing the buffer
	int counter = int(atomicCounterIncrement(ac));


	
	/*d_xyzBuffer[counter*3] = xyz.x;
	d_xyzBuffer[counter*3+1] = xyz.y;
	d_xyzBuffer[counter*3+2] = xyz.z;*/
	float r = (counter/600) / 600.f;	
	d_xyzBuffer[counter*3] = xyz.x;
	d_xyzBuffer[counter*3+1] = xyz.y;
	d_xyzBuffer[counter*3+2] = xyz.z;
	//d_xyzBuffer[counter*3+1] = xyz.y;	
	//d_xyzBuffer[counter*3+2] = xyz.z;
	/*int index = int(counter);
	if(index <= 0)
		index = 0;
	uint idx = uint(index);
	*/
	vec3 color = vec3(r,0.0,0.0);

	gl_FragColor = vec4(color, 1);
}
