#version 440 core
precision mediump float;

layout (location = 0) in vec4 vertex;

out gl_PerVertex
{	
	vec4 gl_Position;
};

out block
{	
	vec4 position;
	vec4 eyePosition;
} Out;

uniform mat4 InvProj;
uniform mat4 InvView;

void main()
{
	//At this stage I need to transform the vertex pos from clip to eye space.
	Out.eyePosition = InvProj * vertex;
	Out.eyePosition /= Out.eyePosition.w;

	Out.position = InvView * Out.eyePosition; // Back to world space.

	gl_Position = vertex;
}