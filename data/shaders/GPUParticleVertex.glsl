#version 410 core

layout (location = 0) in vec3  position;
layout (location = 1) in vec3  velocity;
layout (location = 2) in float spawntime;
layout (location = 3) in float lifespan;

out vec4  Position;
out float SpawnTime;
out float LifeSpan;

void main()
{
	Position	= vec4(position, 1);
	SpawnTime	= spawntime;
	LifeSpan	= lifespan;
}