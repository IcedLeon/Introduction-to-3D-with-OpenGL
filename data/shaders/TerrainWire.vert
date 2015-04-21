#version 440 core
#extension GL_EXT_shader_io_blocks : enable
precision mediump float;

//layout(std140, binding = 1) uniform UniformTess
//{
//	mat4 Projection;
//	mat4 MV;
//	mat4 MVP;
//	mat4 InvProj;
//	mat4 InvView;
//	vec4 EyePosWorld;
//	vec4 Viewport;
//	vec4 FrustumPlanes[6];
//	vec3 TileSize;
//	vec3 GridOrigin;
//	vec3 LightDir;
//	vec3 LightDirWorld;
//	vec2 Translate;
//	float Time;
//	float TileBoundSphereR;
//	float InvFocalLen;
//	float InnerTessFactor;
//	float OuterTessFactor;
//	float NoiseFreq;
//	float InvNoiseSize;
//	float InvNoise3DSize;
//	float HeightScale;
//	float TriSize;
//	int NoiseOctaves;
//	int GridW;
//	int GridH;
//	bool SmoothNormal;
//	bool Cull;
//	bool LOD;
//};

layout (location = 0) in vec4 vertex;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout (location = 1) out block {
    mediump vec2 Tex_Coord;
} Out;

uniform vec3 GridOrigin;
uniform vec3 TileSize;
uniform int GridW;
uniform int GridH;

void main()
{
	//The position patch in 2d grid based on instaced id
	int ix = gl_InstanceID % GridW;
	int iy = gl_InstanceID / GridH;

	Out.Tex_Coord = vec2(float(ix) / float(GridW), float(iy) / float(GridH));

	vec3 pos = GridOrigin + vec3(float(ix) * TileSize.x, 0, float(iy) * TileSize.z);

	gl_Position = vec4(pos, 1.0f);
}