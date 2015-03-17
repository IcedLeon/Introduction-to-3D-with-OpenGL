#version 420 core

layout (location = 0) out vec4 Colour;

layout (binding = 0) uniform sampler2D Tex_Depth;

void main(void)
{
    float d = texelFetch(Tex_Depth, ivec2(gl_FragCoord.xy * 3.0) + ivec2(850, 1050), 0).r;
    d = (d - 0.95) * 15.0;
    Colour = vec4(d);
}
