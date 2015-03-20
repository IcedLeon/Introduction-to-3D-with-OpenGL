#version 410 core

in vec2 Tex_Coord;

out vec4 Out_Colour;

uniform sampler2D Albedo_Tex;
uniform sampler2D Light_Tex;

void main()
{
	vec3 light	= texture(Light_Tex, Tex_Coord).rgb;
	vec3 albedo = texture(Albedo_Tex, Tex_Coord).rgb;

	Out_Colour = vec4(albedo * light, 1);
}