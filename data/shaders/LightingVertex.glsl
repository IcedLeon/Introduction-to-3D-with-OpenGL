#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec4 frag_position;
out vec4 frag_normal;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	frag_position = position;
	frag_normal = normal;
	mat4 projview = Projection * View;
	gl_Position = projview * position;
}