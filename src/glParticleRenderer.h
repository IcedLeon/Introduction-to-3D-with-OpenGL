#ifndef _GLPARTICLERENDER_H_
#define _GLPARTICLERENDER_H_
//Lib
#include "ParticlesRenderer.h"

class GLParticleRenderer : public IParticlesRenderer
{
protected:
	ParticleSystem* m_oSystem;

	unsigned int m_uiBufPos;
	unsigned int m_uiBufCol;
	unsigned int m_uiVAO;
public:
	GLParticleRenderer() : m_oSystem(nullptr),
						   m_uiBufPos(NULL),
						   m_uiBufCol(NULL),
						   m_uiVAO(NULL)
	{ }
	~GLParticleRenderer() { Destroy(); }

	void Generate(ParticleSystem* a_oSys, bool a_bUseQuads) override;
	void Destroy() override;
	void Update() override;
	void Render() override;
};
#endif //!_GLPARTICLERENDER_H_