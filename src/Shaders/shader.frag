
#version 430 compatibility
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_load : enable

in vec3 xyz;

layout(location = 0, index = 0) out vec4 fragColor;

//Grid texture
//uniform layout(size4x32) image3D gridTex;
uniform sampler3D gridTex;

void main(void)
{
	vec3 color = vec3(0.0f);
	float h = 1.0f/256.0f;
	float hz = 1.0f/256.0f;
	vec3 pos = vec3(gl_FragCoord.xy*h,0);
	pos.z = pos.y;
	pos.y = 0;
	//ivec3 pos = ivec3(gl_FragCoord.xy*h*256.0f,0);
	//ivec3 pos = ivec3(128,128,0);

	for(int i = 0; i < 256; ++i) {
		//if(imageLoad(gridTex,pos).r > 0) {
		vec3 val = texture3D(gridTex,pos).xyz;
		//color += vec3(val.z)*h;
		if(val.x > 0.0f) {
			color = vec3(1.0);
			break;
		}
		pos.y += hz;
	}

	
	fragColor = vec4(color,1.0);
}
