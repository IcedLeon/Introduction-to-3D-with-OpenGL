#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;

out vec4 Colour;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	Colour = colour;

	mat4 ProjectionView = Projection * View;

	gl_Position = ProjectionView * position;
}