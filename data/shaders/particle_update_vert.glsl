#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float spawntime;
layout (location = 3) in float lifespan;

out vec3 Position;
out vec3 Velocity;
out float SpawnTime;
out float LifeSpan;

uniform vec3 emitterPos;
uniform float velocityMax;
uniform float velocityMin;
uniform float time;
uniform float deltaTime;
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
	Position = position + velocity * deltaTime;
	Velocity = velocity;
	SpawnTime = SpawnTime + deltaTime;
	LifeSpan = lifespan;

	if (SpawnTime > LifeSpan)
	{
		Position = emitterPos;
		
		uint _seed = uint(gl_VertexID + (time * 1000));

		float _velRange = velocityMax - velocityMin;
		float _speed = rand(_seed++, _velRange) + velocityMin;

		Velocity.x = rand(_seed++, 2) - 1;
		Velocity.y = rand(_seed++, 2) - 1;
		Velocity.z = rand(_seed++, 2) - 1;

		Velocity = normalize(Velocity) * _velRange;

		SpawnTime = 0;

		float _lifeRange = lifeMax - lifeMin;
		LifeSpan = rand(_seed++, _lifeRange) + lifeMin;		 
	}
}