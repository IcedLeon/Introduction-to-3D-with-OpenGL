#version 410 core

in vec2 UVCoord;

out vec4 OutColour;

uniform sampler2D Target;

vec4 Sample()
{
	return texture(Target, UVCoord);
}

vec4 BoxBlur()
{
	vec2 texel_uv = 1.0 / textureSize(Target, 0).xy;
	
	float ninth = 1.0 / 9.0;

	vec4 colour = texture(Target, UVCoord);

	colour += texture(Target, UVCoord + texel_uv) * ninth;
	
	colour += texture(Target, UVCoord + vec2(texel_uv.x,  0) )			 * ninth;
	colour += texture(Target, UVCoord + vec2(texel_uv.x, -texel_uv.y) )  * ninth;
	colour += texture(Target, UVCoord + vec2(0,			  texel_uv.y) )  * ninth;
	colour += texture(Target, UVCoord + vec2(0,			 -texel_uv.y) )  * ninth;	
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, texel_uv.y) )  * ninth;	
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, 0) )			 * ninth;
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, -texel_uv.y) ) * ninth;

	colour /= 9;

	return colour;
}

float e		= 2.71828182846f;
float pi	= 3.14159265359f;
float d		= 0.84089642f;

vec4 GaussianBlur()
{
	vec2 texel_uv = 1.0 / textureSize(Target, 0).xy;

	float quarter	= 0.25;
	float ninth		= 1.0 / 9.0;
	float sixteenth = 1.0 / 16.0;

	vec4 colour = texture(Target, UVCoord) * quarter;

	colour += texture(Target, UVCoord + texel_uv) * ninth;	
	colour += texture(Target, UVCoord + vec2(texel_uv.x,  0) ) * ninth;
	colour += texture(Target, UVCoord + vec2(texel_uv.x, -texel_uv.y) ) * ninth;
	colour += texture(Target, UVCoord + vec2(0,			  texel_uv.y) ) * ninth;

	colour += texture(Target, UVCoord + vec2(0,			 -texel_uv.y) ) * sixteenth;		
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, texel_uv.y) ) * sixteenth;	
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, 0) ) * sixteenth;
	colour += texture(Target, UVCoord + vec2(-texel_uv.x, -texel_uv.y) ) * sixteenth;

	colour /= 9;

	return colour;
}

vec4 Distort()
{	
	vec2 mid = vec2(0.5, 0.5);

	float dist_to_center = distance(UVCoord, mid);

	vec2 normalized_coord = normalize(UVCoord - mid);

	float bias = dist_to_center + sin(dist_to_center * 30.0) * 0.01;

	vec2 new_coord = mid + normalized_coord * bias;

	return texture(Target, new_coord);
}

void main()
{
	OutColour = (Distort() + GaussianBlur() ) / 2;
}