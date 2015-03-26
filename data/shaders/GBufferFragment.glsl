#version 410 core

in vec4 ViewSpace_Position;
in vec4 ViewSpace_Normal;

layout (location = 0) out vec3 Tex_Albedo;
layout (location = 1) out vec3 Tex_Position;
layout (location = 2) out vec3 Tex_Normal;

void main()
{
	Tex_Albedo	 = vec3(1);
	Tex_Position = ViewSpace_Position.xyz;
	Tex_Normal	 = (ViewSpace_Normal.xyz) * 0.5f + 0.5f;
}