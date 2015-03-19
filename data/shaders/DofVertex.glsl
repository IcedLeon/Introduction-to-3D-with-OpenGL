#version 420 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 MV_Matrix;
uniform mat4 Proj_Matrix;

out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

// Position of light
uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = MV_Matrix * position;

    // Calculate normal in view-space
    vs_out.N = mat3(MV_Matrix) * normal;

    // Calculate light vector
    vs_out.L = light_pos - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

    // Calculate the clip-space position of each vertex
    gl_Position = Proj_Matrix * P;
}
