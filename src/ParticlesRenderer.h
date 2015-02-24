#ifndef _PARTICLESRENDERER_H_
#define _PARTICLESRENDERER_H_
//Lib
#include <memory>

//Fwrding
class ParticleSystem;

class IParticlesRenderer
{
public:
	IParticlesRenderer() { }
	virtual ~IParticlesRenderer() { }

	virtual void Generate(ParticleSystem* a_oSys, bool a_bUseQuads) = NULL;
	virtual void Destroy() = NULL;
	virtual void Update() = NULL;
	virtual void Render() = NULL;
};

class RendererFactory
{
public:
	static std::shared_ptr<IParticlesRenderer> Create(const char* a_pccName);
};

#endif //!_PARTICLESRENDERER_H_