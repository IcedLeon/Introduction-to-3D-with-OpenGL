#version 410 core

in vec2 UVCoord;

out vec4 OutColour;

uniform sampler2D Target;

vec4 Simple()
{
	return texture(Target, UVCoord);
}

void main()
{
	OutColour = Simple();
}