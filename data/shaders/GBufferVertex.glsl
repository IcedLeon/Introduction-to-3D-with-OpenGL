#version 410 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

out vec4 ViewSpace_Position;
out vec4 ViewSpace_Normal;

uniform mat4 View_Projection;
uniform mat4 View;

void main()
{
	ViewSpace_Position	= View * vec4(position.xyz, 1);
	ViewSpace_Normal	= View * vec4(normal.xyz, 0);

	gl_Position = View_Projection * position;
}