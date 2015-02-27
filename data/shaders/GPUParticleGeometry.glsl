#version 410 core 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 position[];
in float spawntime[];
in float lifespan[];

out vec4 Colour;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 CameraTransform;

uniform vec4 colourStart;
uniform vec4 colourEnd;

uniform float sizeStart;
uniform float sizeEnd;
//uniform float sizeMod;

void main()
{
	Colour = mix(colourStart, colourEnd, spawntime[0]/lifespan[0]);

	float _halfSize = mix(sizeStart, sizeEnd, spawntime[0]/lifespan[0]) * 0.5; //sizeMod

	vec3 _corners[4];
	_corners[0] = vec3(_halfSize, -_halfSize, 0);
	_corners[1] = vec3(_halfSize, _halfSize, 0);
	_corners[2] = vec3(-_halfSize, -_halfSize, 0);
	_corners[3] = vec3(-_halfSize, _halfSize, 0);

	vec3 _f = normalize(CameraTransform[3].xyz - position[0]);
	vec3 _r = cross(CameraTransform[1].xyz, _f);
	vec3 _u = cross(_f, _r);
	mat3 _billboard = mat3(_r, _u, _f);

	gl_Position = Projection * View * vec4(_billboard * _corners[0] + position[0], 1);
	EmitVertex();
	gl_Position = Projection * View * vec4(_billboard * _corners[1] + position[0], 1);
	EmitVertex();
	gl_Position = Projection * View * vec4(_billboard * _corners[2] + position[0], 1);
	EmitVertex();
	gl_Position = Projection * View * vec4(_billboard * _corners[3] + position[0], 1);
	EmitVertex();
}