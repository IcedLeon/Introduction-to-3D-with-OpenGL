#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;
out vec4 ReflectedScreenPos;

uniform mat4 Projection;
uniform mat4 View;

uniform mat4 PVReflected;
//uniform mat4 Model;

void main()
{
	TexCoord	= texCoord;
	ReflectedScreenPos = PVReflected * position;

	mat4 MVP	= Projection * View;
	gl_Position = MVP * position;
} 