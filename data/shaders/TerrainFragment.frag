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

const vec3 _sunColour = vec3(1.0, 1.0, 0.7);
const vec3 _lightColour = vec3(1.0, 1.0, 0.7) * 1.5;
const vec3 _fogColour = vec3(0.7, 0.8, 1.0) * 0.7;

const float _fogExp = 0.05;

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
    sunAmount = pow(sunAmount, 32.0);
    vec3 fogColour = mix(_fogColour, _sunColour, sunAmount);
    return mix(fogColour, col, fogAmount);
}

//float Noised(vec3 pos)
//{
//	vec3 p = floor(pos);
//	vec3 f = fract(pos);
//	vec2 u = f * f * (3.0 - 2.0 * f);
//	if (HighQl)
//	{
//		vec2 uv = p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
//		vec2 rg = texture2D(Noise_Tex, (uv + 0.5) / 256.0, -100.0).xy;
//	}
//	else
//	{
//		vec2 uv = texture(Noise_Tex, (p + vec2(0.5, 0.5)) / 256.0, -100.0).yx;
//		vec2 rg1 = texture(Noise_Tex, (p + vec2(1.5, 0.5)) / 256.0, -100.0).yx;
//		vec2 rg2 = texture(Noise_Tex, (p + vec2(0.5, 1.5)) / 256.0, -100.0).yx;
//		vec2 rg3 = texture(Noise_Tex, (p + vec2(1.5, 1.5)) / 256.0, -100.0).yx;
//		vec2 rg4 = texture(Noise_Tex, (p + vec2(1.5, 1.5)) / 256.0, -100.0).yx;
//		vec2 rg = mix(mix(rg1, rg2, f.x), mix(rg3, rg4, f.x), f.y);
//	}
//
//	return mix(rg.x, rg.y, f.z);
//}

float Noise(vec2 pos)
{
    return texture(Noise_Tex, pos * InvNoiseSize).x * 2.0 - 1.0;
}

//float Softshadow( in vec3 ro, in vec3 rd, float mint, float k )
//{
//    float res = 1.0;
//    float t = mint;
//    for( int i=0; i<50; i++ )
//    {
//        float h = Map(ro + rd*t).x;
//        res = min( res, k*h/t );
//		t += clamp( h, 0.5, 1.0 );
//		if( h<0.001 ) break;
//    }
//    return clamp(res,0.0,1.0);
//}

// Oren-Nayar
float Diffuse( in vec3 l, in vec3 n, in vec3 v, float r )
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

	float diffuse = 0.0;
    diffuse += 0.2 * Diffuse( normalize(vec3( 0.0, 1.0, 0.0 )), n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse( normalize(vec3( 3.0, 1.0, 0.0 )), n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse( normalize(vec3(-3.0, 1.0, 0.0 )), n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse( normalize(vec3( 0.0, 1.0, 3.0 )), n, -viewDir, 1.0 );
    diffuse += 0.2 * Diffuse( normalize(vec3( 0.0, 1.0,-3.0 )), n, -viewDir, 1.0 );

	//diffuse *= Saturate( (dot(n, -LightDirWorld) + wrap) / (1.0 + wrap));
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

    // fog
    float dist = length(vertexEye);
    //finalColor = ApplyFog(finalColor, dist);
    finalColor = ApplyFog(finalColor, dist, viewDir);
        
	finalColor = pow(finalColor, vec4(1.0 / 2.2).xyz );
	//Vignetting
	vec2 q = gl_FragCoord.xy / Viewport.zw;

	//finalColor *= 0.5 + 0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.3);

    return finalColor;
}

void main()
{
	Frag_Colour = vec4(ColourTerrain(In.vertex.xyz, In.vertexEye.xyz, In.normal), 1.0); //Note: per pixel.
}