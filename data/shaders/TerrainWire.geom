#version 440 core

layout (triangles, invocations = 1) in;

layout (line_strip, max_vertices = 3) out;

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

layout (location = 1) in block
{
	vec3 vertex;
	vec3 vertexEye;
	vec3 normal;
} In[];

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}