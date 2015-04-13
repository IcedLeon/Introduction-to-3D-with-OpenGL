#version 420 core

in vec2 Tex_Coord;

out vec4 Out_Colour;

uniform sampler2D Sample_Tex;
uniform vec3 Text_Colour;

void main()
{   
    vec4 _sampling = vec4(1.0, 1.0, 1.0, texture(Sample_Tex, Tex_Coord).r);
    Out_Colour = vec4(Text_Colour, 1.0) * _sampling;
}