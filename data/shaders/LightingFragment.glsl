#version 410 core

in vec4 Normal;

out vec4 OutNormal;

void main()
{
	OutNormal = vec4(1.0f, 1.0f, 1.0f, 0.5f);
}