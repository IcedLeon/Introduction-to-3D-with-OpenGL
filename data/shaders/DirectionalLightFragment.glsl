#version 410 core

in vec2 Tex_Coord;

out vec3 Out_Light;

uniform vec3 Light_Dir;
uniform vec3 Light_Colour;

uniform sampler2D Position_Tex;
uniform sampler2D Normal_Tex;

void main()
{
	vec3 normalSample = texture(Normal_Tex, Tex_Coord).xyz;
	normalSample *= 2;
	normalSample -= 1;

	vec3 positionSample = texture(Position_Tex, Tex_Coord).xyz;
	
	float d = max(0, dot(normalSample, Light_Dir));

	Out_Light = Light_Colour * d;
}