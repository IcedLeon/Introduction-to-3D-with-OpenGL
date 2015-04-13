#version 420 core

layout (location = 0) out vec4 Frag_Colour;

in block
{
	vec4 position;
	vec4 eyePosition;
} In;

const vec3 _skyColour	= vec3(0.7, 0.8, 1.0) * 0.7;
const vec3 _fogColour	= vec3(0.8, 0.8, 1.0);
const vec3 _cloudColour = vec3(1.0);
const vec3 _sunColour	= vec3(1.0, 1.0, 0.25);

const float _skyHeight = 5.0;
const float _skyTop = 6.0;
const float _cloudStepSize = 1.0;
const float _cloudDensity = 0.25;
const int _cloudSteps = 8;

uniform sampler3D mediump Rand_Tex3D;
uniform vec4 Eye_Pos_World;
uniform vec3 Light_Dir_World;
uniform vec2 Translate;
uniform float Inv_Noise3d_Size;
uniform float Time;
uniform int Octaves = 4;

// returns value clamped in [-1, 1]
float Noise3D(vec3 pos)
{
    return texture(Rand_Tex3D, pos * Inv_Noise3d_Size).x * 2.0 - 1.0;
}

float Turbulence3D(vec3 pos, int octaves = 4, float lacunarity = 2.0, float gain = 0.5)
{
    float sum = 0.0;
    float amp = 0.5;
    for(int i = 0; i < octaves; ++i) 
	{
        sum += abs(Noise3D(pos))*amp;
        pos *= lacunarity;
        amp *= gain;
    }
    return sum;
}

vec4 CloudMap(vec3 pos)
{
	float dist = Turbulence3D(pos * 1.0 + vec3(Time * 0.05, -Time * 0.05, 0), Octaves);
	//Determine the threshold for the cloud density.
	dist = smoothstep(0.2, 0.5, dist);
	//Determine the darknes base on the height.
	float col = smoothstep(_skyHeight, _skyTop, pos.y) * 0.5 + 0.5;

	return vec4(col, col, col, dist * _cloudDensity);
}

vec4 RayMarchClouds(vec3 ro, vec3 rd, float stepSize)
{
	vec4 sum = vec4(0);

	vec3 pos = ro;

	vec3 step = rd * stepSize;

	//Ray march front to back
	for (int i = 0; i < _cloudSteps; ++i)
	{
		vec4 col = CloudMap(pos);
		col.rgb *= col.a; //Pre-multiplycation of Alpha.
		sum = sum + col * (1.0 - sum.a);
		pos += step;
	}
	return sum;
}

float IntersectPlane(vec3 norm, float dist, vec3 ro, vec3 rd)
{
	return (-dist - dot(ro, norm)) / dot(rd, norm);
}

vec4 Skybox(vec3 ro, vec3 rd)
{
	//Intersect ray with the skybox plane.
	float t = IntersectPlane(vec3(0.0, -1.0, 0.0), _skyHeight, ro, rd);

	float tFar = IntersectPlane(vec3(0.0, -1.0, 0.0), _skyTop, ro, rd);

	float stepSize = (tFar - t) / float(_cloudSteps);

	vec4 c = vec4(0.0);

	if (t > 0.0 && rd.y > 0.0)
	{
		vec3 hitPos = ro.xyz + t * rd;
		hitPos.xz += Translate;
		c = RayMarchClouds(hitPos, rd, stepSize);
	}
	//Fade with angle
	c *= smoothstep(0.0, 1.0, rd.y);
	//Adding the Skybox
	vec3 sky = mix(_skyColour, _fogColour, pow(1.0, 1.0 - rd.y), 10.0);
	//Adding the sun under clouds
	float sun = pow(max(0.0, dot(rd, -Light_Dir_World)), 500.0);
	sky += _sunColour * sun;

	col.rgb = col.rgb + sky * (1.0 - c.a);

	return col;
}

void main()
{
	//For the ray calculation must be done in world space.
	vec3 eyePos = Eye_Pos_World.xyz;
	vec3 viewDir = normalize(In.position.xyz - eyePos);

	Frag_Colour = Skybox(eyePos, viewDir);
}