#version 410

in vec4 OutColor;

out vec4 vFragColor;

uniform sampler2D tex;

void main()
{
	vFragColor = texture(tex, gl_PointCoord).rgb * OutColor.rgb, 1);
}