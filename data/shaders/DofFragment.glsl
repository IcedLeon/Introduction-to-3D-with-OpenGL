#version 420 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

// Material properties
uniform vec3 Diffuse_Albedo = vec3(0.9, 0.8, 1.0);
uniform vec3 Specular_Albedo = vec3(0.7);
uniform float Specular_Power = 300.0;
uniform bool Full_Shading = true;

void main(void)
{
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);

    // Calculate R locally
    vec3 R = reflect(-L, N);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(N, L), 0.0) * Diffuse_Albedo;
    vec3 specular = pow(max(dot(R, V), 0.0), Specular_Power) * Specular_Albedo;

    // Write final color to the framebuffer
    color = vec4(diffuse + specular, fs_in.V.z);
}
