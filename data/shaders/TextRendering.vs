#version 420

layout (location = 0) in vec4 verticies;

out vec2 Tex_Coord;

uniform mat4 Ortho_Proj;

void main()
{
	gl_Position = Ortho_Proj * vec4(verticies.xy, 0.0, 1.0);

	Tex_Coord = verticies.zw;
}