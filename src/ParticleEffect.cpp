#include "ParticleEffect.h"

#include <string>
#include "TunnelEffect.h"

std::shared_ptr<IEffect> EffectFactory::Create(const char* a_pccName)
{
	std::string effect{ a_pccName };

	if (effect == "tunnel")
		return std::make_shared<TunnelEffect>();
	//else if (effect == "attractors")
	//	return std::make_shared<AttractorEffect>();
	//else if (effect == "fountain")
	//	return std::make_shared<FountainEffect>();

	return nullptr;
}