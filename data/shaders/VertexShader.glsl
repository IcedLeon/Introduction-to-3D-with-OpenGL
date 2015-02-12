#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 uv_coord;

out vec4 Colour;
out vec2 UV_Coord;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 World;
uniform float time;
uniform float heightScale;
uniform float noise;

void main()
{
	Colour = colour;
	UV_Coord = uv_coord;
	mat4 ProjectionView = Projection * View;
	vec4 P = position;
    P.y = sin(time + position.x) + sin(time + position.z) * heightScale;
	gl_Position = ProjectionView * P;
}