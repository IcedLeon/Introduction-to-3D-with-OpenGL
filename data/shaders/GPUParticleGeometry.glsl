#version 410 core 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4  Position[];
in float SpawnTime[];
in float LifeSpan[];

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
	float _t = SpawnTime[0] / LifeSpan[0];

	Colour = mix(colourStart, colourEnd, _t);

	float _halfSize = mix(sizeStart, sizeEnd, _t) * 0.5; //sizeMod

	vec4 _corners[4];

	_corners[0] = vec4( _halfSize, -_halfSize, 0, 1);
	_corners[1] = vec4( _halfSize,  _halfSize, 0, 1);
	_corners[2] = vec4(-_halfSize, -_halfSize, 0, 1);
	_corners[3] = vec4(-_halfSize,  _halfSize, 0, 1);

	vec3 _f = normalize(CameraTransform[3].xyz - Position[0].xyz);
	vec3 _r = cross(CameraTransform[1].xyz, _f);
	vec3 _u = cross(_f, _r);

	mat4 _billboard = mat4(vec4(_r, 0), 
						   vec4(_u, 0), 
						   vec4(_f, 0), 
						   vec4(0, 0, 0, 1));

	_corners[0] = _billboard * _corners[0] + Position[0];
	_corners[1] = _billboard * _corners[1] + Position[0];
	_corners[2] = _billboard * _corners[2] + Position[0];
	_corners[3] = _billboard * _corners[3] + Position[0];

	mat4 _projview = Projection * View;

	gl_Position = _projview * _corners[0];
	EmitVertex();
	gl_Position = _projview * _corners[1];
	EmitVertex();
	gl_Position = _projview * _corners[2];
	EmitVertex();
	gl_Position = _projview * _corners[3];
	EmitVertex();
}