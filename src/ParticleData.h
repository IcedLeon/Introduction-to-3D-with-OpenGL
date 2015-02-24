#ifndef _PARTICLEDATA_H_
#define _PARTICLEDATA_H_
//Lib
#include <memory>
#include "glm/vec4.hpp"

using std::unique_ptr;
using glm::vec4;

class ParticleData
{
public:
	unique_ptr<vec4[]> m_pvPosition;
	unique_ptr<vec4[]> m_pvColour;
	unique_ptr<vec4[]> m_pvInitColour;
	unique_ptr<vec4[]> m_pvLastColour;
	unique_ptr<vec4[]> m_pvVelocity;
	unique_ptr<vec4[]> m_pvAcceleration;
	unique_ptr<vec4[]> m_pvTime;
	unique_ptr<bool[]> m_pbAlive;

	size_t m_Counter;
	size_t m_AliveCounter;

public:
	ParticleData() : m_pvPosition(nullptr),
					 m_pvColour(nullptr),
					 m_pvInitColour(nullptr),
					 m_pvLastColour(nullptr),
					 m_pvVelocity(nullptr),
					 m_pvAcceleration(nullptr),
					 m_pbAlive(nullptr),
					 m_Counter(NULL),
					 m_AliveCounter(NULL)
	{ }
	explicit ParticleData(size_t a_MaxCount) { Generate(a_MaxCount); }
	~ParticleData() { }

	ParticleData(const ParticleData&) = delete;
	ParticleData& operator=(const ParticleData&) = delete;

	void Generate(size_t a_MaxCount);
	void Destroy(size_t a_ID);
	void Awake(size_t a_ID);
	void SwapData(size_t a_First, size_t a_Second);

	static void CopyOnlyAlive(const ParticleData* a_pcoSource, ParticleData* a_poDestination);
	static size_t ComputeMemoryUsage(const ParticleData& a_roParticle);

};
#endif //!_PARTICLEDATA_H_