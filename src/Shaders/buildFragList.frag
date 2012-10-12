
#version 420

#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_load : enable
//#extension GL_NV_shader_buffer_store : enable

layout(location = 0, index = 0) out vec4 fragColor;


//GPU pointer to xyz buffer
uniform float *d_xyzBuffer;

//Grid texture
coherent uniform layout(size4x32) image3D gridTex;

//Scene bounding volume
uniform vec3 boxMin;
uniform vec3 boxMax;
uniform int gridDim;

layout(binding = 0, offset = 0) uniform atomic_uint ac;

in vec4 f_xyz;
in vec3 f_normal;
in vec2 f_texCoord;
in vec3 f_color;
flat in int f_axis;
flat in vec4 f_AABB;

#define LOW10 0x3FF
#define MID10 0xFFC00
#define HIGH10 0x3FF00000

float
packCoords(ivec3 ijk)
{
	/*int val = 0;
	val = (ijk.x&LOW10)<<20;
	val |= (ijk.y&LOW10)<<10;
	val |= ijk.z&LOW10;
	return intBitsToFloat(val);
	*/
	return intBitsToFloat( (ijk.x&LOW10)<<20 | (ijk.y&LOW10)<<10 | ijk.z&LOW10 );
}

ivec3 
unpack(float val)
{
	int ival = floatBitsToInt(val);
	return ivec3( (ival & HIGH10) >> 20, (ival & MID10) >> 10, ival & LOW10);
}

void main(void)
{

	vec2 pos = f_xyz.xy / f_xyz.w;
	if( pos.x < f_AABB.x || pos.y < f_AABB.y ||
		pos.x > f_AABB.z || pos.y > f_AABB.w)
		discard;
	
	ivec3 ijk;
	if(f_axis == 2)
		ijk = ivec3(gl_FragCoord.xy,gridDim - gl_FragCoord.z*gridDim);
	else
		ijk = ivec3(gl_FragCoord.xy,gl_FragCoord.z*gridDim);
	
	float dz = 0.5*fwidth(gl_FragCoord.z);
	ivec3 ijkMin = ivec3(gl_FragCoord.xy,(gl_FragCoord.z - dz)*gridDim);
	ivec3 ijkMax = ivec3(gl_FragCoord.xy,(gl_FragCoord.z + dz)*gridDim);
	
	if(f_axis == 0) {//proj in yz-plane
		ijk = ijk.zyx;
		ijkMin = ijkMin.zyx;
		ijkMax = ijkMax.zyx;
	} else if(f_axis == 1) { //proj in xz-plane
		ijk = ijk.xzy;	
		ijkMin = ijkMin.xzy;
		ijkMax = ijkMax.xzy;
	}
	
	
	
	//Counter must be int to work properly when indexing the buffer
	int counter = int(atomicCounterIncrement(ac));	
	

	float f = packCoords(ijk);	
	ivec3 un = unpack(f);
	d_xyzBuffer[counter*3] = float(ijk.x);
	d_xyzBuffer[counter*3+1] = float(ijk.y);
	d_xyzBuffer[counter*3+2] = float(ijk.z);
	//imageStore(gridTex, ijk, vec4(1.0));
	//imageStore(gridTex, ijkMin, vec4(ijkMin,1.0));
	//imageStore(gridTex, ijkMax, vec4(ijkMax,1.0));


		
	float r = (counter/600) / 600.f;
	//vec3 color = vec3(r,1.0,0.0);
	//fragColor = vec4(f_color, 0.2);
	fragColor = vec4(1.0);
}