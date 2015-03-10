#ifndef _PARTICLESH_H_
#define _PARTICLESH_H_

#include "Application.h"
#include "ShaderCompiler.h"
#include "EmitterSh.h"

class ParticleSh : public Application
{
private:
	ShaderCompiler m_oShader;
	EmitterSh m_oEmitter;

public:
	ParticleSh();
	~ParticleSh();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();
};
#endif //!_PARTICLESH_H_