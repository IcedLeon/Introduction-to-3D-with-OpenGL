#version 420 core

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

uniform mat4 Inverse_Proj;
uniform mat4 Inverse_View;

void main()
{
	//At this stage we need to transform the vertex pos from clip to eye space.
	Out.eyePosition = Inverse_Proj * vertex;
	Out.eyePosition /= Out.eyePosition.w;

	Out.position = Inverse_View * Out.eyePosition; // Back to world space.

	gl_Position = vertex;
}