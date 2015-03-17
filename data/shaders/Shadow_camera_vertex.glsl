#version 420 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 MV_Matrix;
uniform mat4 Proj_Matrix;
uniform mat4 Shadow_Matrix;
uniform vec3 Light_Pos;

out VS_OUT
{
    vec4 shadow_coord;
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = MV_Matrix * position;

    // Calculate normal in view-space
    vs_out.N = mat3(MV_Matrix) * normal;

    // Calculate light vector
    vs_out.L = Light_Pos - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

    // Light-space coordinates
    vs_out.shadow_coord = Shadow_Matrix * position;

    // Calculate the clip-space position of each vertex
    gl_Position = Proj_Matrix * P;
}
