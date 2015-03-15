#version 410 core

in vec2 TexCoord;
in vec4 ReflectedScreenPos;

out vec4 OutColour;

uniform sampler2D Diffuse;

void main()
{
	vec4 uv_pos = ReflectedScreenPos / ReflectedScreenPos.w;
	uv_pos = (uv_pos + 1) * 0.5f;
	OutColour = texture(Diffuse, uv_pos.xy); //texture(Diffuse, TexCoord); 
}