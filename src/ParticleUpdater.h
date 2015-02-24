#ifndef _PARTICLEUPDATER_H_
#define _PARTICLEUPDATER_H_

//class forwarding
class ParticleData;

class ParticleUpdater
{
public:
	ParticleUpdater() { }
	~ParticleUpdater() { }

	virtual void Update(double a_dDeltaTime, ParticleData* a_oParticle) = 0;

};
#endif //!_PARTICLEUPDATER_H_