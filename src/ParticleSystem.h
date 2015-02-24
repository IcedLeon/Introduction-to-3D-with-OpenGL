#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_
//Lib
#include "ParticleData.h"
#include <vector>

using std::vector;
using std::shared_ptr;

//Class forwarding
class ParticleEmitter;
class ParticleUpdater;

class ParticleSystem
{
protected:
	ParticleData m_oParticles;
	ParticleData m_oAliveParticles;
	
	size_t m_Count;
	
	vector <shared_ptr<ParticleEmitter>> m_voEmitters;
	vector <shared_ptr<ParticleUpdater>> m_voUpdaters;

public:
	explicit ParticleSystem(size_t a_MaxCount);
	virtual ~ParticleSystem() { }

	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;

	virtual void Update(double a_dDeltaTime);
	virtual void Reset();

	virtual size_t AllTheParticles() const { return m_oParticles.m_Counter;  }
	virtual size_t AllTheParticlesAlive() const { return m_oAliveParticles.m_Counter; }

	void AddEmitter(shared_ptr<ParticleEmitter> a_Emitter) { m_voEmitters.push_back(a_Emitter); }
	void AddUpdater(shared_ptr<ParticleUpdater> a_Updater) { m_voUpdaters.push_back(a_Updater); }

	ParticleData* GetParticle() { return &m_oParticles; }

	static size_t ComputeMemoryUsage(const ParticleSystem& a_oParticle);
};
#endif //!_PARTICLESYSTEM_H_