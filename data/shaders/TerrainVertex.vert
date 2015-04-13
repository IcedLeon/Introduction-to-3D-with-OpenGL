#version 420 core
#extension GL_EXT_shader_io_blocks : enable

layout (location = 0) in vec4 vertex;

uniform vec3 GridOrigin;
uniform vec3 TileSize;
uniform int GridW;
uniform int GridH;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout (location = 1) out block {
    mediump vec2 Tex_Coord;
} Out;

void main()
{
	//The position patch in 2d grid based on instaced id
	int ix = gl_InstanceID % GridW;
	int iy = gl_InstanceID / GridH;

	Out.Tex_Coord = vec2(float(ix) / float(GridW), float(iy) / float(GridH));

	vec3 pos = GridOrigin + vec3(float(ix) * TileSize.x, 0, float(iy) * TileSize.z);

	gl_Position = vec4(pos, 1.0f);
}