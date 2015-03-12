#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	TexCoord = texCoord;
	mat4 MVP = Projection * View; //* Model
	gl_Position = MVP * position;
} 