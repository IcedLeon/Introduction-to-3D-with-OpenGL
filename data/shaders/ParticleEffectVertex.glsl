#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
//layout (location = 2) in vec2 tex_coord;

out vec4 OutColor;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() 
{
	mat4 modelview = View * Model;

    vec4 eyePos = modelview * vec4(position.xy, 0.5, 1);
	//vec4 projCorner = Projection * vec4(0.5*tex_coord.x, 0.5*tex_coord.x, eyePos.zw);
    gl_Position = Projection * eyePos;

	OutColor = colour;
	
	float dist = length(eyePos.xyz);
	float att	= inversesqrt(0.1f * dist);
	gl_PointSize = 2.0f * att;
	//gl_PointSize = 1024 * projCorner.x / projCorner.w;
}