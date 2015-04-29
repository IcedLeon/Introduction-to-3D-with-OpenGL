#version 440 core

in block
{
	vec4 pos;
	vec4 norm;
	vec4 tan;
	vec4 biTan;
	vec2 uvCoord;
} In;

out vec4 Frag_Colour;

uniform sampler2D Diffuse_Tex;
uniform sampler2D Normal_Tex;
uniform sampler2D Specular_Tex;
uniform vec4 EyePosWorld;
uniform vec3 LightDirWorld;

const vec3 lightColour = vec3(1.0, 1.0, 0.7) * 1.5;
const vec3 ambientColour = vec3(0.5, 0.5, 0.5);
const float shininess = 15;

void main()
{
	//renamed for convenience
	vec3 N = In.norm.xyz;
	N = normalize(N);

	vec3 TA = In.tan.xyz;
	TA = normalize(TA);

	vec3 BTA = In.biTan.xyz;
	BTA = normalize(BTA);
	//Tangent space matrix
	mat3 TBN = mat3(TA, BTA, N);

	//Sampling texture
	vec3 materialCol = texture(Diffuse_Tex, In.uvCoord).xyz;
	vec3 adjustedNorm = texture(Normal_Tex, In.uvCoord).xyz * 2 - 1;
	vec3 specularMap = texture(Specular_Tex, In.uvCoord).xyz;
	//Light
	//Ambient
	vec3 ambient = materialCol * ambientColour;
	//In world space
	vec3 viewDir = normalize(EyePosWorld.xyz - In.pos.xyz);
	//Re-normalize normals since interpolation would change that.
	N	   = normalize(TBN * adjustedNorm);	
	//Diffuse
	vec3 L = normalize(LightDirWorld);
	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColour * materialCol;
	//Specular
	vec3 R = reflect(L, N);
	float s = max(0.0, dot(R, viewDir));
	s = pow(s, shininess);
	vec3 specular = vec3(s) * lightColour * specularMap;

	Frag_Colour = vec4(ambient + diffuse + specular, 1.0);
}

//void main()
//{
//	//renamed for convenience
//	vec3 N = In.norm.xyz;
//	N = normalize(N);
//
//	vec3 TA = In.tan.xyz;
//	TA = normalize(TA);
//
//	vec3 BTA = In.biTan.xyz;
//	BTA = normalize(BTA);
//
//	mat3 TBN = mat3(TA, BTA, N);
//	//Sampling texture
//	vec3 adjustedNorm = texture(Normal_Tex, In.uvCoord).xyz * 2 - 1;
//	vec3 materialCol = texture(Diffuse_Tex, In.uvCoord).xyz;
//	//Blinn-Phong//
//	//Ambient
//	vec3 ambient = materialCol * shininess;
//	//Diffuse
//	vec3 L = normalize(-LightDirWorld);
//	//N = normalize(TBN * adjustedNorm);
//	float diff = max(dot(L, N), 0.0);
//	vec3 diffuse = materialCol * diff;
//	//Specular
//	vec3 viewDir = normalize(EyePosWorld.xyz - In.pos.xyz);
//	vec3 reflectDir = reflect(-LightDirWorld, N);
//	float specPwr = 0.0;
//
//	vec3 halfDir = normalize(LightDirWorld + viewDir);
//	specPwr = pow(max(dot(N, halfDir), 0.0), 8.0);
//	vec3 specularMap = texture(Specular_Tex, In.uvCoord).xyz;
//	vec3 specular = lightColour * specularMap * specPwr;
//
//	Frag_Colour = vec4(ambient + diffuse + specular, 1.0);
//}