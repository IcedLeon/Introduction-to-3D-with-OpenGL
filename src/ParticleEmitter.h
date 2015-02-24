#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_
//Lib
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class ParticleData;
class ParticleGenerator;

class ParticleEmitter
{
protected:
	vector<shared_ptr<ParticleGenerator>> m_Generators;
public:
	float m_fEmitterRate;
public:
	ParticleEmitter() : m_fEmitterRate(NULL)
	{

	}
	~ParticleEmitter() {}
	
	virtual void Emit(double a_fDeltaTime, ParticleData* a_poParticle);

	void AddGenerator(shared_ptr<ParticleGenerator> a_Generator)
	{
		m_Generators.push_back(a_Generator);
	}
};
#endif //!_PARTICLEEMITTER_H_