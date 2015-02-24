#include "ParticlesRenderer.h"
#include "glParticleRenderer.h"
#include <string>

std::shared_ptr<IParticlesRenderer> RendererFactory::Create(const char* a_pccName)
{
	std::string _renderer{ a_pccName };

	if (_renderer == "gl")
	{
		return std::make_shared<GLParticleRenderer>();
	}

	return nullptr;
}