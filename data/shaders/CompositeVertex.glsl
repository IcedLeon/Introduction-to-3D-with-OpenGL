#version 410 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv_coord;

out vec2 Tex_Coord;

void main()
{
	gl_Position = position;
	Tex_Coord = uv_coord;
}