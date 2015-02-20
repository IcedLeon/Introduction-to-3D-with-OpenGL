#version 410 core

in vec2 frag_text_coord;

out vec4 Frag_colour;

uniform sampler2D diffuse_tex;

void main()
{
	Frag_colour = texture(diffuse_tex, frag_text_coord);
}