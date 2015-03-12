#ifndef _GPUPARTICLE_H_
#define _GPUPARTICLE_H_

#include "Application.h"
#include "ShaderCompiler.h"

class GPUEmitter;

class GPUParticle : public Application
{
private:
	GPUEmitter*	m_oEmitter;
	float m_fTimer;

public:
	GPUParticle();
	~GPUParticle();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 6.0f, 5.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();
};
#endif //!_GPUPARTICLE_H_