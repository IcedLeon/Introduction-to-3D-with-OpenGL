#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float spawntime;
layout (location = 3) in float lifespan;

out vec3  Updated_Position;
out vec3  Updated_Velocity;
out float Updated_SpawnTime;
out float Updated_LifeSpan;

uniform vec3 emitterPos;

uniform float time;
uniform float deltaTime;

uniform float velocityMin;
uniform float velocityMax;

uniform float lifeMin;
uniform float lifeMax;

const float INVERSE_MAX_UINT = 1.0 / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;

	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);

	return float(range * i) * INVERSE_MAX_UINT;
} 

void main()
{
	Updated_Position  = position + velocity * deltaTime;
	Updated_Velocity  = velocity;
	Updated_SpawnTime = spawntime + deltaTime;
	Updated_LifeSpan  = lifespan;

	if (Updated_SpawnTime > Updated_LifeSpan)
	{
		Updated_Position   = emitterPos;
		
		uint _seed		   = uint(gl_VertexID + (time * 1000.0f));

		float _velRange	   = velocityMax - velocityMin;
		float _speed	   = rand(_seed++, _velRange) + velocityMin;

		Updated_Velocity.x = rand(_seed++, 2) - 1;
		Updated_Velocity.y = rand(_seed++, 2) - 1;
		Updated_Velocity.z = rand(_seed++, 2) - 1;

		Updated_Velocity   = normalize(Updated_Velocity) * _velRange;

		Updated_SpawnTime  = 0;

		float _lifeRange   = lifeMax - lifeMin;
		Updated_LifeSpan   = rand(_seed++, _lifeRange) + lifeMin;		 
	}
}