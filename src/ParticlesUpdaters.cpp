#include "ParticlesUpdaters.h"
#include <assert.h>
#include <algorithm>
#include "glm/common.hpp"
#include "glm/gtc/random.hpp"

void EulerUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	const vec4 _globalAcc{ a_dDeltaTime * m_vGlobalAcceleration.x,
						   a_dDeltaTime * m_vGlobalAcceleration.y,
						   a_dDeltaTime * m_vGlobalAcceleration.z,
						   0.0f };

	const float _localDT = (float)a_dDeltaTime;

	vec4* __restrict _acc = a_oParticle->m_pvAcceleration.get();
	vec4* __restrict _vel = a_oParticle->m_pvVelocity.get();
	vec4* __restrict _pos = a_oParticle->m_pvPosition.get();

	const unsigned int _endID = a_oParticle->m_AliveCounter;

	for (size_t i = NULL; i < _endID; ++i)
	{
		_acc[i] += _globalAcc;
	}

	for (size_t i = NULL; i < _endID; ++i)
	{
		_vel[i] += _localDT * _acc[i];
	}

	for (size_t i = NULL; i < _endID; ++i)
	{
		_pos[i] += _localDT * _vel[i];
	}
}

void FloorUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	const float _localDT = (float)a_dDeltaTime;

	vec4* __restrict _acc = a_oParticle->m_pvAcceleration.get();
	vec4* __restrict _vel = a_oParticle->m_pvVelocity.get();
	vec4* __restrict _pos = a_oParticle->m_pvPosition.get();

	const size_t _endID = a_oParticle->m_AliveCounter;

	for (size_t i = 0; i < _endID; ++i)
	{
		if (_pos[i].y < m_fFloorY)
		{
			vec4 _force = a_oParticle->m_pvAcceleration[i];
			float _normalFactor = glm::dot(_force, vec4(0.0f, 1.0f, 0.0f, 0.0f));

			if (_normalFactor < 0.0f)
			{
				_force -= vec4(0.0f, 1.0f, 0.0f, 0.0f) * _normalFactor;
			}
			float _velFactor = glm::dot(_vel[i], vec4(0.0f, 1.0f, 0.0f, 0.0f));
			_vel[i] -= vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_fBounceFactor) * _velFactor;

			_acc[i] = _force;
		}
	}
}

void AttractorUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	const float _localDT = (float)a_dDeltaTime;

	vec4* __restrict _acc = a_oParticle->m_pvAcceleration.get();
	vec4* __restrict _vel = a_oParticle->m_pvVelocity.get();
	vec4* __restrict _pos = a_oParticle->m_pvPosition.get();

	const size_t _endID = a_oParticle->m_AliveCounter;	
	const size_t _attractorCounter = m_vAttractors.size();

	vec4 _offset;
	float _dist;
	size_t _a;

	for (size_t i = 0; i < _endID; ++i)
	{
		for (_a = NULL; _a < _attractorCounter; ++_a)
		{
			_offset.x = m_vAttractors[_a].x - _pos[i].x;
			_offset.y = m_vAttractors[_a].y - _pos[i].y;
			_offset.z = m_vAttractors[_a].z - _pos[i].z;


			_dist = glm::dot(_offset, _offset);

			_dist = m_vAttractors[_a].w / _dist;

			_acc[i] += _offset * _dist;
		}
	}
}

void BasicColourtUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	vec4* __restrict _col		= a_oParticle->m_pvColour.get();
	vec4* __restrict _initCol	= a_oParticle->m_pvInitColour.get();
	vec4* __restrict _endCol	= a_oParticle->m_pvLastColour.get();
	vec4* __restrict _time		= a_oParticle->m_pvTime.get();

	const size_t _endID = a_oParticle->m_AliveCounter;

	for (size_t i = 0; i < _endID; ++i)
	{
		_col[i] = glm::mix(_initCol[i], _endCol[i], _time[i].z);
	}
}

void PosColourUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	vec4* __restrict _col = a_oParticle->m_pvColour.get();
	vec4* __restrict _initCol = a_oParticle->m_pvInitColour.get();
	vec4* __restrict _endCol = a_oParticle->m_pvLastColour.get();
	vec4* __restrict _time = a_oParticle->m_pvTime.get();
	vec4* __restrict _pos = a_oParticle->m_pvPosition.get();

	const size_t _endID = a_oParticle->m_AliveCounter;

	float _scaleR,
		_scaleG,
		_scaleB;

	float _diffR = m_vMaxPosition.x - m_vMinPosition.x;
	float _diffG = m_vMaxPosition.y - m_vMinPosition.y;
	float _diffB = m_vMaxPosition.z - m_vMinPosition.z;

	for (unsigned int i = 0; i < _endID; ++i)
	{
		_scaleR = (_pos[i].x - m_vMinPosition.x) / _diffR;
		_scaleG = (_pos[i].y - m_vMinPosition.y) / _diffG;
		_scaleB = (_pos[i].z - m_vMinPosition.z) / _diffB;

		_col[i].r = _scaleR;
		_col[i].g = _scaleG;
		_col[i].b = _scaleB;
		_col[i].a = glm::mix(_initCol[i].a, _endCol[i].a, _time[i].w);
	}
}

void VelColourUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	vec4* __restrict _col = a_oParticle->m_pvColour.get();
	vec4* __restrict _initCol = a_oParticle->m_pvInitColour.get();
	vec4* __restrict _endCol = a_oParticle->m_pvLastColour.get();
	vec4* __restrict _time = a_oParticle->m_pvTime.get();
	vec4* __restrict _pos = a_oParticle->m_pvPosition.get();

	const size_t _endID = a_oParticle->m_AliveCounter;

	float _scaleR,
		_scaleG,
		_scaleB;

	float _diffR = m_vMaxVelocity.x - m_vMinVelocity.x;
	float _diffG = m_vMaxVelocity.y - m_vMinVelocity.y;
	float _diffB = m_vMaxVelocity.z - m_vMinVelocity.z;

	for (size_t i = 0; i < _endID; ++i)
	{
		_scaleR = (_pos[i].x - m_vMinVelocity.x) / _diffR;
		_scaleG = (_pos[i].y - m_vMinVelocity.y) / _diffG;
		_scaleB = (_pos[i].z - m_vMinVelocity.z) / _diffB;

		_col[i].r = _scaleR;
		_col[i].g = _scaleG;
		_col[i].b = _scaleB;
		_col[i].a = glm::mix(_initCol[i].a, _endCol[i].a, _time[i].w);
	}
}

void BasicTimeUpdater::Update(double a_dDeltaTime, ParticleData* a_oParticle)
{
	unsigned int _endID = a_oParticle->m_AliveCounter;
	const float _localDT = (float)a_dDeltaTime;

	vec4* __restrict _time = a_oParticle->m_pvTime.get();

	if (_endID == NULL)
	{
		return;
	}

	for (size_t i = 0; i < _endID; ++i)
	{
		_time[i].x -= _localDT;
		_time[i].z = (float)1.0 - (_time[i].x * _time[i].w);

		if (_time[i].x < (float)1.0)
		{
			a_oParticle->Destroy(i);
			_endID = a_oParticle->m_AliveCounter < a_oParticle->m_Counter ? a_oParticle->m_AliveCounter : a_oParticle->m_Counter;
		}
	}
}