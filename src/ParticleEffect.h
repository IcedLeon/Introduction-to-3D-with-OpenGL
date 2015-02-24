#ifndef _PARTICLEEFFECT_H_
#define _PARTICLEEFFECT_H_
//Lib
#include <memory>

class IEffect
{
public:
	static const size_t DEFAULT_PARTICLE_NUM_FLAG = NULL;	// for initialize method

	IEffect() { }
	virtual ~IEffect() { }

	// creates the effect with desired num of particles, (NULL means default for the effect)
	virtual bool Initialize(size_t numParticles) = NULL;
	virtual bool InitializeRenderer() = NULL;
	virtual void Reset() = NULL;
	virtual void Clean() = NULL;
	virtual void AddUI() = NULL;
	virtual void RemoveUI() = NULL;

	virtual void Update(double a_dDeltaTime) = NULL;
	virtual void CpuUpdate(double a_dDeltaTime) = NULL;
	virtual void GpuUpdate(double a_dDeltaTime) = NULL;
	virtual void Render() = NULL;

	virtual int AllParticles() = NULL;
	virtual int AllAliveParticles() = NULL;
};

class EffectFactory
{
public:
	static std::shared_ptr<IEffect> Create(const char* a_pccName);
};
#endif //!_PARTICLEEFFECT_H_