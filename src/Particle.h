#ifndef _PARTICLE_H_
#define _PARTICLE_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include "ParticleEffect.h"

class Particle : public Application
{
private:
	ShaderCompiler			 m_oShader;
	std::shared_ptr<IEffect> m_oEffects;
	IEffect*				 m_oCurrentEffect;
	GLuint					 m_uiTexture;
	GLint					 m_iCurrentEffectID;
	GLint					 m_iAllParticle;
	GLint					 m_iAliveParticle;

public:
	Particle() {}
	~Particle() {}

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void LoadTexture();
};
#endif //!_PARTICLE_H_