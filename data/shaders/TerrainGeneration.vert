#version 440 core
#extension GL_EXT_shader_io_blocks : enable
precision mediump float;

layout (location = 0) in vec4 vertex;
layout (location = 8) in vec2 texCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out block
{	
	mediump vec2 Tex_Coord;
} Out;

void main()
{
	Out.Tex_Coord = texCoord;
	gl_Position = vertex;
}