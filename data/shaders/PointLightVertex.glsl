#version 410 core

layout (location = 0) in vec4 position;

uniform vec3 Light_Pos;
uniform float Light_Radius;
uniform mat4 Proj_View;

void main()
{
	vec3 _result = position.xyz * Light_Radius + Light_Pos;
	gl_Position = Proj_View * vec4(_result.xyz, 1);
}