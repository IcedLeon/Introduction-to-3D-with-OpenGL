#version 440 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 tangent;
layout (location = 3) in vec4 bone_index;
layout (location = 4) in vec4 bone_weights;
layout (location = 8) in vec2 texCoord;

out block
{
	vec4 pos;
	vec4 norm;
	vec4 tan;
	vec4 biTan;
	vec2 uvCoord;
} Out;

const int MAX_BONES = 128;

uniform mat4 VP;
uniform mat4 Model;
uniform mat4 Bones[MAX_BONES];

void main()
{
	Out.uvCoord = texCoord;

	ivec4 indices = ivec4(bone_index);

	vec4 Pos = vec4(0.0);
	vec4 Tan = vec4(0.0);
	vec4 Nor = vec4(0.0);

	Pos += Bones[indices.x] * position * bone_weights.x;
	Pos += Bones[indices.y] * position * bone_weights.y;
	Pos += Bones[indices.z] * position * bone_weights.z;
	Pos.w = 1;
	
	Tan += Bones[indices.x] * tangent * bone_weights.x;
	Tan += Bones[indices.y] * tangent * bone_weights.y;
	Tan += Bones[indices.z] * tangent * bone_weights.z;
	Tan.w = 0;
	
	Nor += Bones[indices.x] * normal * bone_weights.x;
	Nor += Bones[indices.y] * normal * bone_weights.y;
	Nor += Bones[indices.z] * normal * bone_weights.z;
	Nor.w = 0;

	Out.biTan	= vec4(cross(Nor.xyz, Tan.xyz), 0.0);
	Out.norm	= (Model * Nor);
	Out.tan		= (Model * Tan);

	gl_Position = VP * Model * Pos;
	Out.pos		= gl_Position;
}