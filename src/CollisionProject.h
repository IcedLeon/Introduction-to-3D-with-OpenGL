#ifndef _COLLISIONPROJECT_H_
#define _COLLISIONPROJECT_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

class CollisionP : public Application
{
private:
	//ShaderCompiler m_oShader;
	float m_fTimer;

public:
	CollisionP();
	~CollisionP();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GetFrustumPlanes(const mat4& a_mTransform, vec4* a_vPlanes);
};
#endif //!_COLLISIONPROJECT_H_