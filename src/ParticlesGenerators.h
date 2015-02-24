#ifndef _PARTICLESGENERETORS_H_
#define _PARTICLESGENERETORS_H_
//Lib
#include "ParticleGenerator.h"
#include "ParticleData.h"
#include <vector>

class BoxPosGen : public ParticleGenerator
{
public:
	vec4 m_vPosition;
	vec4 m_vMaxStartPosOffset;
public:
	BoxPosGen() : m_vPosition(vec4(0.0f)), m_vMaxStartPosOffset(vec4(0.0f)) { }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class RoundPosGen : public ParticleGenerator
{
public:
	vec4 m_vCenter;
	float m_fRadX;
	float m_fRadY;
public:
	RoundPosGen() : m_vCenter(vec4(0.0f)), m_fRadX(0.0f), m_fRadY(0.0f) { }
	RoundPosGen(const vec4 &a_vCenter, double a_dRadX, double a_dRadY)
		: m_vCenter(a_vCenter), 
		m_fRadX((float)a_dRadX), 
		m_fRadY((float)a_dRadY)
	{ }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class BasicColorGen : public ParticleGenerator
{
public:
	vec4 m_vMinStartCol;
	vec4 m_vMaxStartCol;
	vec4 m_vMinEndCol;
	vec4 m_vMaxEndCol;
public:
	BasicColorGen() : m_vMinStartCol(vec4(0.0f)), m_vMaxStartCol(vec4(0.0f)), m_vMinEndCol(vec4(0.0f)), m_vMaxEndCol(vec4(0.0f)) { }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class BasicVelGen : public ParticleGenerator
{
public:
	vec4 m_vMinStartVel;
	vec4 m_vMaxStartVel;
public:
	BasicVelGen() : m_vMinStartVel(vec4(0.0f)), m_vMaxStartVel(vec4(0.0f)) { }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class SphereVelGen : public ParticleGenerator
{
public:
	float m_fMinVel;
	float m_fMaxVel;
public:
	SphereVelGen() : m_fMinVel(0.0f), m_fMaxVel(0.0f) { }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class VelFromPosGen : public ParticleGenerator
{
public:
	vec4 m_vOffset;
	float m_fMinScale;
	float m_fMaxScale;
public:
	VelFromPosGen() : m_vOffset(vec4(0.0f)), m_fMinScale(0.0f), m_fMaxScale(0.0f) { }
	VelFromPosGen(const glm::vec4 &a_vOff, double a_fMinS, double a_fMaxS)
		: m_vOffset(a_vOff), 
		m_fMinScale((float)a_fMinS),
		m_fMaxScale((float)a_fMaxS)
	{ }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

class BasicTimeGen : public ParticleGenerator
{
public:
	float m_fMinTime;
	float m_fMaxTime;
public:
	BasicTimeGen() : m_fMinTime(0.0f), m_fMaxTime(0.0f) { }

	virtual void Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId) override;
};

#endif //!_PARTICLESGENERETORS_H_