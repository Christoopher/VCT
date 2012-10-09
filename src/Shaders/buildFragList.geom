//GEOMETRY SHADER
 #version 420
 #extension GL_ARB_geometry_shader4 : enable


 #define FLT_MAX 99999999999.0

 //A triangle with three points come in
 layout(triangles) in;

 //Out of three triangle points, we create a triangle strip with two triangles
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform int gridDim;

in vec3 g_xyz[3];
in vec3 g_normal[3];
in vec2 g_texCoord[3];

out vec4 f_xyz;
out vec3 f_normal;
out vec2 f_texCoord;
out vec3 f_color;
flat out int f_axis;
flat out vec4 f_AABB;


 vec4 addPoint(vec4 pos, vec4 bbBounds) {
	if (pos.w < 0) {
		bbBounds.xy = min(bbBounds.xy,pos.xy);
		bbBounds.zw = max(bbBounds.zw,pos.xy);
	} else {
		bbBounds.xy = min(bbBounds.xy,pos.xy/pos.w);
		bbBounds.zw = max(bbBounds.zw,pos.xy/pos.w);
	}
		
	return bbBounds;
}


void main()
 {
	
	mat4 rot2X = mat4(
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f,1.0f,0.0f,0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f,0.0f, 0.0f, 1.0f
	);

	mat4 rot2Y = mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);
	
	vec3 normal = g_normal[0]+g_normal[1]+g_normal[2];
	normal *= 0.33333;	
	vec3 absNormal = abs(normal);	
	int axis = 0;
	float maxVal = absNormal.x;
	if(absNormal.y > maxVal) {
		maxVal = absNormal.y;
		axis = 1;
	}
	if(absNormal.z > maxVal) {		
		axis = 2;
	}
	
	//Defualt rot if we should project on Z-Axis   RED
	mat4 rot = mat4(1.0f,0.0f,0.0f,0.0f,
					0.0f,1.0f,0.0f,0.0f,
					0.0f,0.0f,1.0f,0.0f,
					0.0f,0.0f,0.0f,1.0f);
	f_color = vec3(1.0f,0.0f,0.0f);
	if(axis == 0) {
		rot = rot2X; //Project on X-Axis     GREEN
		f_color = vec3(0.0f,1.0f,0.0f);
	} else if(axis == 1) {
		f_color = vec3(0.0f,0.0f,1.0f);
		rot = rot2Y; //Project on Y-Axis     BLUE
	}

	//if(axis == 0) {		
		vec4 hPixel = vec4(1.0f/gridDim);
		vec4 projPos[3];
		vec4 finalPos;
		vec4 brBounds = vec4(FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX);

		int i;
		for(i=0; i< gl_VerticesIn; i++)
		{			
			projPos[i] = projectionMatrix * rot * gl_PositionIn[i];			
		}	

		//Calc AABB in clip space
		brBounds = addPoint(projPos[0], brBounds);
		brBounds = addPoint(projPos[1], brBounds);
		brBounds = addPoint(projPos[2], brBounds);
		f_AABB = brBounds + vec4(-hPixel.xy, hPixel.xy);

		vec3 planes[2];
		
		//Vertex 0
		planes[0] = cross(projPos[0].xyw - projPos[2].xyw, projPos[2].xyw);
		planes[1] = cross(projPos[1].xyw - projPos[0].xyw, projPos[0].xyw);	
		planes[0].z -= dot(hPixel.xy, abs(planes[0].xy));
		planes[1].z -= dot(hPixel.xy, abs(planes[1].xy));
		
		finalPos.xyw = cross(planes[0],planes[1]);
		finalPos.xyw /= abs(finalPos.w);
		finalPos.z = projPos[0].z;

		gl_Position = finalPos;
		//gl_Position = projPos[0];
		f_xyz = finalPos;

		f_normal = g_normal[0];
		f_texCoord = g_texCoord[0];
		f_axis = axis;
		EmitVertex();	


		//Vertex 1
		planes[0] = cross(projPos[1].xyw - projPos[0].xyw, projPos[0].xyw);
		planes[1] = cross(projPos[2].xyw - projPos[1].xyw, projPos[1].xyw);	
		planes[0].z -= dot(hPixel.xy, abs(planes[0].xy));
		planes[1].z -= dot(hPixel.xy, abs(planes[1].xy));			
		finalPos.xyw = cross(planes[0],planes[1]);
		finalPos.xyw /= abs(finalPos.w);
		finalPos.z = projPos[1].z;

		gl_Position = finalPos;
		//gl_Position = projPos[1];
		f_xyz = finalPos;
		
		f_normal = g_normal[1];
		f_texCoord = g_texCoord[1];
		f_axis = axis;	
		EmitVertex();

		//Vertex 2
		planes[0] = cross(projPos[2].xyw - projPos[1].xyw, projPos[1].xyw);
		planes[1] = cross(projPos[0].xyw - projPos[2].xyw, projPos[2].xyw);	
		planes[0].z -= dot(hPixel.xy, abs(planes[0].xy));
		planes[1].z -= dot(hPixel.xy, abs(planes[1].xy));
		finalPos.xyw = cross(planes[0],planes[1]);
		finalPos.xyw /= abs(finalPos.w);
		finalPos.z = projPos[2].z;

		gl_Position = finalPos;
		//gl_Position = projPos[2];
		f_xyz = finalPos;
		
		f_normal = g_normal[2];
		f_texCoord = g_texCoord[2];
		f_axis = axis;	
		EmitVertex();
		
		EndPrimitive();
	//}

}

