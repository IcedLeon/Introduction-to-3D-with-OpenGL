#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
//layout (location = 2) in vec4 colour;
layout (location = 2) in float spawntime;
layout (location = 3) in float lifespan;

out vec3 Position;
out float SpawnTime;
out float LifeSpan;

void main()
{
	Position = position;
	SpawnTime = spawntime;
	LifeSpan = lifespan;
}