#version 410 core

out vec3 Light_Output;

uniform vec3 Light_View_Pos;
uniform vec3 Light_Diffuse;
uniform float Light_Radius;

uniform sampler2D Position_Tex;
uniform sampler2D Normal_Tex;

void main()
{
	//We compute the texture coordinate.
	vec2 _texCoord			= gl_FragCoord.xy / textureSize(Position_Tex, 0).xy;
	//We sample the texture.
	vec3 _normalSample		= texture(Normal_Tex, _texCoord).xyz;
	_normalSample		   *= 2;
	_normalSample		   -= 1;

	vec3 _positionSample	= texture(Position_Tex, _texCoord).xyz;
	//Now it's time to compute the light direction.
	vec3 _toLight			= Light_View_Pos - _positionSample;
	//Compute lambertian term.
	vec3 _L					= normalize(_toLight);
	float _d				= max(0, dot(_normalSample, _L));
	//Falloff the light source.
	float _dist = length(_toLight) / Light_Radius;
	float _fallOff			= 1 - min(1, _dist);
	//Output the final light
	//Changed to look at the box
	Light_Output			= Light_Diffuse * _d;// * _fallOff;
}