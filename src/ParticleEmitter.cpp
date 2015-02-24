#include "ParticleEmitter.h"
#include "ParticleData.h"
#include "ParticleGenerator.h"
#include <algorithm>

void ParticleEmitter::Emit(double a_fDeltaTime, ParticleData* a_poParticle)
{
	const size_t _maxParticles = static_cast<size_t>(a_fDeltaTime * m_fEmitterRate);
	const size_t _startID = a_poParticle->m_AliveCounter;
	const size_t _endID = std::min(_startID + _maxParticles, a_poParticle->m_Counter - 1);

	for (auto& gen : m_Generators)
	{
		gen->Generate(a_fDeltaTime, a_poParticle, _startID, _endID);
	}
	for (size_t i = _startID; i < _endID; ++i)
	{
		a_poParticle->Awake(i);
	}
}