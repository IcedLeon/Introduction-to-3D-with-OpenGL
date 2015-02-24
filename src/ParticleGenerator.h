#ifndef _PARTICLEGENERATOR_H_
#define _PARTICLEGENERATOR_H_
//lib

class ParticleData;

class ParticleGenerator
{
public:
	ParticleGenerator() { }
	~ParticleGenerator() { }

	virtual void Generate(double a_dDeltaTime, ParticleData* a_poParticle, size_t a_StartID, size_t a_EndID) = 0;
};
#endif //!_PARTICLEGENERATOR_H_