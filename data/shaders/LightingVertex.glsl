#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec4 frag_position;
out vec4 frag_normal;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	frag_position = position;
	frag_normal = normal;
	mat4 MVP = Projection * View; //* Model;
	gl_Position = MVP * position; //* vec4(vertex.x, vertex.y, 0.0, 1.0);
	tex_coord = vertex.zw;
}