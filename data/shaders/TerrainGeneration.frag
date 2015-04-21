#version 440 core
precision mediump float;

in block
{
	mediump vec2 Tex_Coord;
} In;

layout(location=0) out vec4 Frag_Colour;

uniform mediump sampler2D Noise_Tex;
uniform vec3 GridOrigin;
uniform vec3 TileSize;
uniform vec3 Translate;
uniform float InvNoiseSize;
uniform float HeightScale;
uniform float NoiseFreq;
uniform int NoiseOctaves;
uniform int GridW;
uniform int GridH;

//uniform sampler2D Terrain_Tex1;
//uniform sampler2D Terrain_Tex2;
//
//float Terrain(vec2 qu)
//{
//	float th = smoothstep( 0.0, 0.7, texture2D(Terrain_Tex1, 0.001 * qu, -100.0 ).x );
//    float rr = smoothstep( 0.1, 0.5, texture2D(Terrain_Tex2, 2.0*0.03 * qu, -100.0 ).y );
//	float h = 1.9;
//	if(!HighQl)
//	{
//		h += (1.0 - 0.6 * rr) * (1.5 - 1.0 * th) * 0.2 * (1.0 - texture2D(Terrain_Tex1, 0.03 * qu, -100.0 ).x);
//	}
//	h += th * 7.0;
//    h += 0.3 * rr;
//    return -h;
//}

//Smooth interpolation curve
vec2 Fade(vec2 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); //(quintic)
}

//Derivative of fade function
vec2 DerFade(vec2 t)
{
    return 30.0 * t * t * (t * (t - 2.0) + 1.0); //(quintic)
}

vec3 DerNoise(vec2 pos)
{
    vec2 i = floor(pos);
    vec2 u = pos - i;

    vec2 du = DerFade(u);
    u = Fade(u);

    //Sampling neighbouring noise values
    vec4 n = textureGather(Noise_Tex, i * InvNoiseSize) * 2.0 - 1.0;

    //Renamed components for convenience
    float a = n.w;
    float b = n.z;
    float c = n.x;
    float d = n.y;

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = a - b - c + d;

    vec3 r;
    // noise derivative
    r.xy = (vec2(k1, k2) + k3 * u.yx) * du;
    // noise value
    r.z = k0 + k1 * u.x + k2 * u.y + k3 * u.x * u.y;
    return r;
}

//Rotate octaves to avoid axis-aligned artifacts
const mat2 _rotateMat = mat2(1.6, -1.2, 1.2, 1.6);

float FbmGrad(vec2 pos, int octaves = 4, float lacunarity = 2.0, float gain = 0.5)
{
	float amp = 0.5;
	vec2 d = vec2(0.0);
	float sum = 0.0;
	for (int i = 0; i < octaves; ++i)
	{
		vec3 n = DerNoise(pos);
		d += n.xy;
		sum += n.z * amp / (1.0 + dot(d, d)); //Sum scaled by the gradient.
		amp *= gain;
		pos = _rotateMat * pos;
	}
	return sum;
}

float Terrain(vec2 pos)
{
	pos += Translate.xz;
	pos *= NoiseFreq;

	float h = FbmGrad(pos, NoiseOctaves) + 0.2;

	h *= HeightScale;

	const float _waterLevl = 0.05;

	float land = smoothstep(_waterLevl, _waterLevl + 0.2, h);

	h *= 0.1 + 0.9 * land;

	return h;
}

void main()
{
	vec3 pos = GridOrigin + vec3(In.Tex_Coord.x * float(GridW) * TileSize.x, 0.0, In.Tex_Coord.y * float(GridH) * TileSize.z);

	float h = Terrain(pos.xz);
	pos.y = h;

	//Normal
	vec2 triSize = TileSize.xz / 64.0;
	vec3 posDx = pos.xyz + vec3(triSize.x, 0.0, 0.0);
	vec3 posDz = pos.xyz + vec3(0.0, 0.0, triSize.y);
	posDx.y = Terrain(posDx.xz);
	posDz.y = Terrain(posDz.xz);

	vec3 n = normalize(cross(posDz - pos.xyz, posDx - pos.xyz));

	Frag_Colour = vec4(n, h);
}