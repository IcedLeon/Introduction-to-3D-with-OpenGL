#version 440 core
precision mediump float;

layout (location = 1) in block
{
	vec3 vertex;
    vec3 vertexEye;
    vec3 normal;
} In;

layout(location = 0) out vec4 Frag_Colour;

uniform mediump sampler2D Noise_Tex;
uniform	vec3 LightDirWorld;
uniform vec4 EyePosWorld;
uniform vec4 Viewport;
uniform float InvNoiseSize;

float Saturate(float val)
{
	return clamp(val, 0.0, 1.0);
}

float LinearStep(float a, float b, float x)
{
	return Saturate((x - a) / (b - a));
}

const vec3 _sunColour = vec3(0.7, 0.7, 0.7);
const vec3 _lightColour = vec3(1.0, 1.0, 0.7) * 1.5;
const vec3 _fogColour = vec3(0.7, 0.8, 1.0) * 0.7;

const float _fogExp = 0.02;

vec3 ApplayFog(vec3 col, float dist)
{
	float fogAmmount = exp(-dist * _fogExp);
	return mix(_fogColour, col, fogAmmount);
}

//Fog scattering effect.
vec3 ApplyFog(vec3 col, float dist, vec3 viewDir)
{
    float fogAmount = exp(-dist * _fogExp);
    float sunAmount = max( dot(viewDir, LightDirWorld), 0.0);
    sunAmount = pow(sunAmount, 12.0);
    vec3 fogColour = mix(_fogColour, _sunColour, sunAmount) / 1.09;
    return mix(fogColour, col, fogAmount);
}

float Noise(vec2 pos)
{
    return texture(Noise_Tex, pos * InvNoiseSize).x * 2.0 - 1.0;
}

float Softshadow(vec3 ro, vec3 rd, float height, float mint, float k )
{
    float res = 1.0;
    float t = mint;
    for(int i = 0; i < 50; i++ )
    {
        float h = height;
		h = max(h, 0.0);
        res = min( res, k * h / t );
		t += clamp(h, 0.025, 0.5);
		if( h < 0.0001 ) break;
    }
    return clamp(res, 0.0, 1.0);
}

// Oren-Nayar
float Diffuse(vec3 l, vec3 n, vec3 v, float r )
{
	
    float r2 = r * r;
    float a = 1.0 - 0.5 * (r2 / (r2 + 0.57));
    float b = 0.45 * (r2 / (r2 + 0.09));

    float nl = dot(n, l);
    float nv = dot(n, v);

    float ga = dot(v - n * nv, n - n * nl);

	return max(0.0, nl) * (a + b * max(0.0, ga) * sqrt((1.0 - nv * nv) * (1.0 - nl * nl)) / max(nl, nv));
}

vec3 ColourTerrain(vec3 vertex, vec3 vertexEye, vec3 normal)
{
	const float shininess = 100.0f;
	const vec3 ambientColour = vec3(0.05, 0.05, 0.15);
	const float wrap = 0.3;

	vec3 rockColour = vec3(0.4, 0.4, 0.4);
	vec3 snowColour = vec3(0.9, 0.9, 1.0);
	vec3 grassColour = vec3(77.0 / 255.0, 100.0 / 255.0, 42.0 / 255.0);
	vec3 brownColour = vec3(82.0 / 255.0, 70.0 / 255.0, 30.0 / 255.0);
	vec3 waterColour = vec3(0.2, 0.4, 0.5);
	vec3 treeColour = vec3(0.0, 0.2, 0.0);

	vec3 noisePos = vertex.xyz;
	float noiseS = Noise(noisePos.xz) * 0.5 + 0.5;

	float height = vertex.y;

	//Snowy 
	float snowLine = 0.7;
	float isSnow = LinearStep(snowLine, snowLine + 0.1, height * (0.5 + 0.5 * normal.y) );

	//Lighting.
	//In world space
	vec3 viewDir = normalize(EyePosWorld.xyz - vertex);
	vec3 h = normalize(-LightDirWorld + viewDir);
	vec3 n = normalize(normal);
	float sun = pow(max( dot(viewDir, LightDirWorld), 0.0), 12.0);
	float shad = 0.0;
	vec3 finalShad;
	if (sun > 0.01)
	{
		shad = Softshadow(vertex, LightDirWorld, h.x, 0.01, 32.0);
		finalShad = vec3(shad);
	}

	float diffuse = 0.0;
    diffuse += 0.2 * Diffuse(-LightDirWorld, n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse(-LightDirWorld, n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse(-LightDirWorld, n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse(-LightDirWorld, n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse(-LightDirWorld, n, -viewDir, 1.0 );

	//diffuse = Saturate( (dot(n, -LightDirWorld) + wrap) / (1.0 + wrap));
	float specular = pow(Saturate(dot(h, n)), shininess);
	
    grassColour = mix(grassColour * 0.25, grassColour * 1.5, noiseS);
    brownColour = mix(brownColour * 0.035, brownColour * 1.5, noiseS);
	
    //Choose material color based on height and normal

    vec3 matColor;
    matColor = mix(rockColour, grassColour, LinearStep(0.6, 0.8, normal.y));
    matColor = mix(matColor, brownColour, LinearStep(0.9, 1.0, normal.y ));
    // snow
    matColor = mix(matColor, snowColour, isSnow);

    float isWater = smoothstep(0.05, 0.0, height);
    matColor = mix(matColor, waterColour, isWater);

    vec3 finalColor = ambientColour * matColor + diffuse * matColor * 
	_lightColour + specular * _lightColour * isWater;

	//finalColor += sun * vec3(1.64, 1.27, 0.99);// * pow(finalShad, vec3(1.0, 1.2, 1.5));
    // fog
    float dist = length(vertexEye);
    finalColor = ApplyFog(finalColor, dist, viewDir);
    //Gamma correction.
	finalColor = pow(finalColor, vec4(1.0 / 2.2).xyz);
	//Contrast
	finalColor = finalColor * 0.3 + 0.7 * finalColor * finalColor *(3.0 - 2.0 * finalColor);
	//Desaturation
	finalColor = mix(finalColor, vec3(finalColor.x + finalColor.y + finalColor.z) * 0.33, 0.2);
	//Tint
	finalColor *= 1.3 * vec3(1.06, 1.1, 1.0);
	//Vignetting
	vec2 q = gl_FragCoord.xy / Viewport.zw;
	finalColor *= 0.5 + 0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.3);

    return finalColor;
}

void main()
{
	Frag_Colour = vec4(ColourTerrain(In.vertex.xyz, In.vertexEye.xyz, In.normal), 0.0); //Note: per pixel.
}