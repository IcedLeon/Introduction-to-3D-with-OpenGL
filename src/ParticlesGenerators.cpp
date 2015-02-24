#include "ParticlesGenerators.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

#ifndef M_PI

#define M_PI 		3.1415926535897932384626433832795f
#define M_2_PI 		6.28318530717958647692528676655901f		// PI*2

#endif

void BoxPosGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	vec4 _posMin{ m_vPosition.x - m_vMaxStartPosOffset.x,
		m_vPosition.y - m_vMaxStartPosOffset.y,
		m_vPosition.z - m_vMaxStartPosOffset.z,
		1.0 };

	vec4 _posMax{ m_vPosition.x + m_vMaxStartPosOffset.x,
		m_vPosition.y + m_vMaxStartPosOffset.y,
		m_vPosition.z + m_vMaxStartPosOffset.z,
		1.0 };

	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		a_poParticle->m_pvPosition[i] = glm::linearRand(_posMin, _posMax);
	}
}

void RoundPosGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		double _ang = glm::linearRand(0.0, M_PI * 2.0);
		a_poParticle->m_pvPosition[i] = m_vCenter + vec4(m_fRadX * sinf((float)_ang), m_fRadY * cosf((float)_ang), 0.0f, 1.0f);
	}
}

void BasicColorGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		a_poParticle->m_pvInitColour[i] = glm::linearRand(m_vMinStartCol, m_vMaxStartCol);
		a_poParticle->m_pvLastColour[i] = glm::linearRand(m_vMinEndCol, m_vMaxEndCol);
	}
}

void BasicVelGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		a_poParticle->m_pvVelocity[i] = glm::linearRand(m_vMinStartVel, m_vMaxStartVel);
	}
}

void SphereVelGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	float _phi, 
		_theta, 
		_v, 
		_r;

	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		_phi	= glm::linearRand(-M_PI, M_PI);
		_theta	= glm::linearRand(-M_PI, M_PI);
		_v		= glm::linearRand(m_fMinVel, m_fMaxVel);

		_r = _v * sinf(_phi);
		a_poParticle->m_pvVelocity[i].z = _v * cosf(_phi);
		a_poParticle->m_pvVelocity[i].x = _r * cosf(_theta);
		a_poParticle->m_pvVelocity[i].y = _r * sinf(_theta);
	}
}

void VelFromPosGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		float _scale = static_cast<float>(glm::linearRand(m_fMinScale, m_fMaxScale));
		vec4 _vel = (a_poParticle->m_pvPosition[i] - m_vOffset);
		a_poParticle->m_pvVelocity[i] = _scale * _vel;
	}
}

void BasicTimeGen::Generate(double a_fDeltaTime, ParticleData* a_poParticle, size_t a_StartId, size_t a_EndId)
{
	for (size_t i = a_StartId; i < a_EndId; ++i)
	{
		a_poParticle->m_pvTime[i].x = a_poParticle->m_pvTime[i].y = glm::linearRand(m_fMinTime, m_fMaxTime);
		a_poParticle->m_pvTime[i].z = (float)0.0;
		a_poParticle->m_pvTime[i].w = (float)1.0 / a_poParticle->m_pvTime[i].x;
	}
}