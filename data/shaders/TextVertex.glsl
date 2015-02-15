#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 vertex;

out vec4 frag_position;
out vec4 frag_normal;
out vec2 tex_coord;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	frag_position = position;
	frag_normal = normal;
	mat4 projview = Projection * View;
	gl_Position = projview * position * vec4(vertex.xy, 0.0, 1.0);
	tex_coord = vertex.zw;
}