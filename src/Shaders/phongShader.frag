#version 330
out vec4 vFragColor;
in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
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
}
	