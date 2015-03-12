#version 410 core

in vec2 TexCoord;

out vec4 OutColour;

uniform sampler2D Diffuse;

void main()
{
	OutColour = texture(Diffuse, TexCoord);
}