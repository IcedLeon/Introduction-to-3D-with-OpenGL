#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleUpdater.h"

ParticleSystem::ParticleSystem(size_t a_MaxCount)
{
	m_Count = a_MaxCount;
	m_oParticles.Generate(a_MaxCount);
	m_oAliveParticles.Generate(a_MaxCount);

	for (size_t i = NULL; i < a_MaxCount; ++i)
	{
		m_oParticles.m_pbAlive[i] = false;
	}
}

void ParticleSystem::Update(double a_dDeltaTime)
{
	for (auto& em : m_voEmitters)
	{
		em->Emit(a_dDeltaTime, &m_oParticles);
	}
	for (size_t i = NULL; i < m_Count; ++i)
	{
		m_oParticles.m_pvAcceleration[i] = vec4(0.0f);
	}

	for (auto& updt : m_voUpdaters)
	{
		updt->Update(a_dDeltaTime, &m_oParticles);
	}
}

void ParticleSystem::Reset()
{
	m_oParticles.m_AliveCounter = NULL;
}

size_t ParticleSystem::ComputeMemoryUsage(const ParticleSystem& a_roParticle)
{
	return 2 * ParticleData::ComputeMemoryUsage(a_roParticle.m_oParticles);
}