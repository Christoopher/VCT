//GEOMETRY SHADER
 #version 420
 #extension GL_ARB_geometry_shader4 : enable


layout(points, invocations = 4) in;
layout(line_strip, max_vertices = 4) out;

in int gl_InvocationID;

uniform vec3[8] corners;
uniform ivec4[4] connectivity;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform float scale;

void main()
{
	vec3 pos = gl_PositionIn[0].xyz;
	ivec4 con = connectivity[gl_InvocationID];

	gl_Position = projectionMatrix * modelViewMatrix * vec4(pos - vec3(10) + 0.5*vec3(scale) + corners[con.x]*scale,1);
	EmitVertex();

	gl_Position = projectionMatrix * modelViewMatrix * vec4(pos - vec3(10) + 0.5*vec3(scale) + corners[con.y]*scale,1);
	EmitVertex();

	gl_Position = projectionMatrix * modelViewMatrix * vec4(pos - vec3(10) + 0.5*vec3(scale) + corners[con.z]*scale,1);
	EmitVertex();

	gl_Position = projectionMatrix * modelViewMatrix * vec4(pos - vec3(10) + 0.5*vec3(scale) + corners[con.w]*scale,1);
	EmitVertex();

	EndPrimitive();	
}

