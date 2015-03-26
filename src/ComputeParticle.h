#ifndef _COMPUTEPARTICLE_H_
#define _COMPUTEPARTICLE_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

class ComputeParticle : public Application
{
private:

public:
	ComputeParticle();
	~ComputeParticle();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 40.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	GLuint CreateNoise3D(int a_iWidth, int a_iHeight, int a_iDist, GLint a_iInternalFormat);
};
#endif //!_COMPUTEPARTICLE_H_