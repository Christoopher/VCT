#version 420
out vec4 vFragColor;
in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec3 vFragPos;
in vec3 ambientNormal;

//Grid texture
//layout(binding = 0) uniform image3D gridTex;
uniform sampler3D gridTex;

void main(void)
 {
 
	vFragColor = vec4(0.0f);
	vec4 ambientColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4 diffuseColor = vec4(0.3f);
	vec4 specularColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// Dot product gives us diffuse intensity
	float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));
	// Multiply intensity by diffuse color, force alpha to 1.0
	vFragColor = diff * diffuseColor;
	// Add in ambient light
	vFragColor += ambientColor;
	// Specular Light
	vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
	float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
	if(diff != 0.0) {
		float fSpec = pow(spec, 10.0);
		vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
	}
	vec3 pos = 0.005*vFragPos;
	pos += vec3(10);
	pos /= 20.0;



	/*float h = 1.0/256;
	float bitValue = 0;
	bitValue = texture(gridTex,pos).x;//imageLoad(gridTex,ivec3(vFragPos)).x;
	bitValue -= texture(gridTex,vec3(pos.x+h, pos.y, pos.z)).x /7.0;
	bitValue -= texture(gridTex,vec3(pos.x-h, pos.y, pos.z)).x /7.0;
	bitValue -= texture(gridTex,vec3(pos.x, pos.y+h, pos.z)).x /7.0;
	bitValue -= texture(gridTex,vec3(pos.x, pos.y-h, pos.z)).x /7.0;
	bitValue -= texture(gridTex,vec3(pos.x, pos.y, pos.z+h)).x /7.0;
	bitValue -= texture(gridTex,vec3(pos.x, pos.y, pos.z-h)).x /7.0;
	*/

	float ambient = 0;
	int nrOfSteps = 10;
	float h = 1.0/256;
	vec3 normal = normalize(ambientNormal)/512;
	for(int i=0; i < nrOfSteps; ++i) //RAY 1, UP
	{
		pos += normal;
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 2 
	{
		pos += vec3(normal.x+h, normal.y, normal.z);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 3
	{
		pos += vec3(normal.x-h, normal.y, normal.z);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 4
	{
		pos += vec3(normal.x, normal.y+h, normal.z);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 5
	{
		pos += vec3(normal.x+h, normal.y-h, normal.z);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 6
	{
		pos += vec3(normal.x+h, normal.y, normal.z+h);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}
	for(int i=0; i < nrOfSteps; ++i) //RAY 7
	{
		pos += vec3(normal.x+h, normal.y, normal.z-h);
		if(texture(gridTex,pos).x > 0)
		{
			ambient += 1/(1+i);
			break;
		}
	}


	vFragColor.rgb = vec3(1.0);
	 vFragColor.rgb -= ambient*0.1;
}
	