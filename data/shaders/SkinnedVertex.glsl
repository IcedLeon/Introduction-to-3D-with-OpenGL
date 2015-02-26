#version 410 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec4 bone_indices;
layout (location = 3) in vec4 bone_weights;
layout (location = 4) in vec4 normal;
layout (location = 5) in vec4 tex_tan;
layout (location = 6) in vec4 tex_cotan;

out vec4 frag_position;
out vec4 frag_normal;
out vec2 frag_text_coord;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 World;

const int MAX_BONES = 128;
uniform mat4 Bones[MAX_BONES];

void main()
{
	frag_text_coord = tex_coord;
	
	ivec4 indices = ivec4(bone_indices);

	vec4 final_position = vec4(0.0, 0.0, 0.0, 0.0);

	final_position += Bones[indices.x] * position * bone_weights.x;
																
	final_position += Bones[indices.y] * position * bone_weights.y;
																
	final_position += Bones[indices.z] * position * bone_weights.z;
																
	final_position += Bones[indices.w] * position * bone_weights.w;

	final_position.w = 1.0;

	mat4 MVP = Projection * View * World;

	gl_Position = MVP * final_position;
}