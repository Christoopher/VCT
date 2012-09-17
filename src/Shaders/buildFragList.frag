
#version 420

#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_load : enable
//#extension GL_NV_shader_buffer_store : enable


//GPU pointer to xyz buffer
uniform float *d_xyzBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint ac;

in vec3 f_xyz;
in vec3 f_normal;
in vec2 f_texCoord;
in vec3 f_color;

void main(void)
{
	//Counter must be int to work properly when indexing the buffer
	int counter = int(atomicCounterIncrement(ac));

	d_xyzBuffer[counter*3  ] = f_xyz.x;
	d_xyzBuffer[counter*3+1] = f_xyz.y;
	d_xyzBuffer[counter*3+2] = f_xyz.z;
	
	//float r = (counter/600) / 600.f;	
	//vec3 color = vec3(r,1.0,0.0);
	gl_FragColor = vec4(f_color, 1);
}
