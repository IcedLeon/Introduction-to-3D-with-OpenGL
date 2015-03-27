#version 410 core

in vec2 Out_UV_Coord;

out vec4 Out_Colour;

uniform sampler2D Perlin_Tex;

void main()
{
	Out_Colour = texture(Perlin_Tex, Out_UV_Coord).rrrr;
	Out_Colour.a = 1;
}