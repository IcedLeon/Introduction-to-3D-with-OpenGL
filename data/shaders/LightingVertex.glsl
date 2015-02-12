#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec4 Normal;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	Normal = normal;
	mat4 projview = Projection * View;
	gl_Position = projview * position;
}