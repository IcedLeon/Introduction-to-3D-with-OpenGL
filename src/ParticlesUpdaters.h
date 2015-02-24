#ifndef _PARTICLESUPDATERS_H_
#define _PARTICLESUPDATERS_H_
//Lib
#include "ParticleUpdater.h"
#include "ParticleData.h"
#include <vector>

using std::vector;

class EulerUpdater : public ParticleUpdater
{
public:
	vec4 m_vGlobalAcceleration;

	EulerUpdater() : m_vGlobalAcceleration(vec4(0.0f))
	{
	
	}
	~EulerUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle);
};

class FloorUpdater : public ParticleUpdater
{
public:
	float m_fFloorY;
	float m_fBounceFactor;

	FloorUpdater() : m_fFloorY(0.0f),
					 m_fBounceFactor(0.0f)
	{ }

	~FloorUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle);
};

class AttractorUpdater : public ParticleUpdater
{
protected:
	vector<vec4> m_vAttractors; //.w is the force.
public:
	AttractorUpdater() { }
	~AttractorUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle);
	
	size_t CollectionSize() const { return m_vAttractors.size(); }
	void Attractor(const vec4& a_oAttractor) { m_vAttractors.push_back(a_oAttractor); }
	vec4& GetAttractor(size_t a_ID) { return m_vAttractors[a_ID]; }
};

class BasicColourtUpdater : public ParticleUpdater
{
public:
	BasicColourtUpdater() { }
	~BasicColourtUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle) override;
};

class PosColourUpdater : public ParticleUpdater
{
public:
	vec4 m_vMinPosition;
	vec4 m_vMaxPosition;

	PosColourUpdater() : m_vMinPosition(vec4(0.0f)),
						 m_vMaxPosition(vec4(0.0f))
	{ }
	~PosColourUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle) override;
};

class VelColourUpdater : public ParticleUpdater
{
public:
	vec4 m_vMinVelocity;
	vec4 m_vMaxVelocity;

	VelColourUpdater() : m_vMinVelocity(vec4(0.0f)),
						 m_vMaxVelocity(vec4(0.0f))
	{ }
	~VelColourUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle) override;
};

class BasicTimeUpdater : public ParticleUpdater
{
public:
	BasicTimeUpdater() { }
	~BasicTimeUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle) override;
};

#endif //!_PARTICLESUPDATERS_H_