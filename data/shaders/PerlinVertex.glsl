#version 410 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv_coord;

out vec2 Out_UV_Coord;

uniform mat4 Proj_Trans;
uniform mat4 View_Trans;

uniform sampler2D Perlin_Tex;

uniform float Scale_Mul;

void main()
{
	vec4 _pos = position;
	_pos.y += texture(Perlin_Tex, uv_coord).r * Scale_Mul;

	Out_UV_Coord = uv_coord;
	mat4 _VP = Proj_Trans * View_Trans;
	gl_Position = _VP * _pos;
}