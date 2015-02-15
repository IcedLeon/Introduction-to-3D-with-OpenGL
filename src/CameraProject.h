#ifndef _PROJECTCAMERA_H_
#define _PROJECTCAMERA_H_

#include "Application.h"
#include "Camera.h"

const int PLANET_COUNT = 10;

class CameraProject : public Application
{
private:
	GLfloat m_fTimer;

public:
	CameraProject();
	~CameraProject();
	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = true);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void Draw();
	virtual void Use() {}
	virtual void CleanUpWin();

	mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot);
	void MoveCamera(float a_fDeltaTime);
};
#endif //!_PROJECTCAMERA_H_