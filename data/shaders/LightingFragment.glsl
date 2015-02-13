#version 410 core

in vec4 frag_normal;
in vec4 frag_position;

out vec4 frag_color;

uniform vec3 light_direction;
uniform vec3 light_colour;
uniform vec3 material_colour;	
uniform vec3 ambient_light;
uniform vec3 eye_pos;
uniform float specular_power;

void main()
{
	vec3 ambient = material_colour * ambient_light;
	
	vec3 N = normalize(frag_normal.xyz);
	vec3 L = normalize(light_direction);
	
	float d = max(0.0, dot(N, -L));
	
	vec3 diffuse = vec3(d) * light_colour * material_colour;
		
	vec3 E = normalize(eye_pos - frag_position.xyz);
	vec3 R = reflect(L, N);
	
	float s = dot(R, E);
	s = pow(s, specular_power);
	
	vec3 specular = vec3(s) * light_colour * material_colour;	
	
	frag_color = vec4(ambient + diffuse + specular, 1);
}