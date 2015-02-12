#version 410 core

in vec4 Colour;
in vec2 UV_Coord;

out vec4 PixelColour;

uniform sampler2D diffuse;
uniform float modColor;

void main()
{
	PixelColour = vec4(modColor, modColor, modColor, modColor);
	PixelColour.rgb -= texture(diffuse, UV_Coord).rgb;
};