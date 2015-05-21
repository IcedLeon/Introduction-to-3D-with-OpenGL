#version 420 core

in vec4 a_vertex;
in vec3 a_normal;
in vec2 a_texCoord;

out vec3 v_normal;
out vec2 v_texCoord;
out vec3 v_eye;

uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;
0.

void main()
{
	vec4 vertex = u_modelViewMatrix * a_vertex;

	v_eye = -vec3(vertex);

	v_normal = u_normalMatrix * a_normal;
		
	v_texCoord = a_texCoord;
		
	gl_Position = u_projectionMatrix * vertex;
}    