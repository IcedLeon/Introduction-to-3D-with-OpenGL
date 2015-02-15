#ifndef _PROJECTOPENGL1_H_
#define _PROJECTOPENGL1_H_
//Libs
#include "Application.h"

const int PLANET_COUNT = 5;

class Project1 : public Application
{
private:

	GLfloat m_fTimer;
	GLfloat m_fCameraX;
	GLfloat m_fCameraZ;

public:
	Project1();
	~Project1();
	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void Draw();
	virtual void CleanUpWin();

	mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot);
};
#endif //!_PROJECTOPENGL1_H_