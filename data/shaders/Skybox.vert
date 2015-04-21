#version 440 core
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