#include "ParticleData.h"

void ParticleData::Generate(size_t a_MaxCount)
{
	m_Counter		= a_MaxCount;
	m_AliveCounter	= NULL;

	m_pvPosition.reset(new vec4[a_MaxCount]);
	m_pvColour.reset(new vec4[a_MaxCount]);
	m_pvInitColour.reset(new vec4[a_MaxCount]);
	m_pvLastColour.reset(new vec4[a_MaxCount]);
	m_pvVelocity.reset(new vec4[a_MaxCount]);
	m_pvAcceleration.reset(new vec4[a_MaxCount]);
	m_pvTime.reset(new vec4[a_MaxCount]);
	m_pbAlive.reset(new bool[a_MaxCount]);
}

void ParticleData::Destroy(size_t a_ID)
{
	if (m_AliveCounter > 0)
	{
		m_pbAlive[a_ID] = false;
		SwapData(a_ID, m_AliveCounter - 1);
		m_AliveCounter--;
	}
}

void ParticleData::Awake(size_t a_ID)
{
	if (m_AliveCounter < m_Counter)
	{
		m_pbAlive[a_ID] = true;
		//SwapData(a_ID, m_AliveCounter);
		m_AliveCounter++;
	}
}

void ParticleData::SwapData(size_t a_First, size_t a_Second)
{
	m_pvPosition[a_First] = m_pvPosition[a_Second];
	m_pvColour[a_First] = m_pvColour[a_Second];
	m_pvInitColour[a_First] = m_pvInitColour[a_Second];
	m_pvLastColour[a_First] = m_pvLastColour[a_Second];
	m_pvVelocity[a_First] = m_pvVelocity[a_Second];
	m_pvAcceleration[a_First] = m_pvAcceleration[a_Second];
	m_pvTime[a_First] = m_pvTime[a_Second];
	//m_pbAlive[a_First] = m_pbAlive[a_Second];
}

void ParticleData::CopyOnlyAlive(const ParticleData* a_pcoSource, ParticleData* a_poDestination)
{
	assert(a_pcoSource->m_Counter == a_poDestination->m_Counter);

	size_t _ID = NULL;

	for (size_t i = NULL; i < a_pcoSource->m_AliveCounter; ++i)
	{
		a_poDestination->m_pvPosition[_ID]		= a_pcoSource->m_pvPosition[i];
		a_poDestination->m_pvColour[_ID]		= a_pcoSource->m_pvColour[i];
		a_poDestination->m_pvInitColour[_ID]	= a_pcoSource->m_pvInitColour[i];
		a_poDestination->m_pvLastColour[_ID]	= a_pcoSource->m_pvLastColour[i];
		a_poDestination->m_pvVelocity[_ID]		= a_pcoSource->m_pvVelocity[i];
		a_poDestination->m_pvAcceleration[_ID]	= a_pcoSource->m_pvAcceleration[i];
		a_poDestination->m_pvTime[_ID]			= a_pcoSource->m_pvTime[i];
		a_poDestination->m_pbAlive[_ID]			= true;
		_ID++;
	}
	a_poDestination->m_AliveCounter = _ID;
}

size_t ParticleData::ComputeMemoryUsage(const ParticleData& a_roParticle)
{
	return a_roParticle.m_Counter * (7 * sizeof(vec4) + sizeof(bool)) + sizeof(size_t) * 2;
}