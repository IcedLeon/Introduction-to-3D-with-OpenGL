#ifndef _PROJECTCAMERA_H_
#define _PROJECTCAMERA_H_

#include "Application.h"
#include "Camera.h"

const int PLANET_COUNT = 10;

class CameraProject : public Application
{
private:
	static Camera m_oCamera;
	mat4 m_oProjection;
	mat4 m_oView;
	GLfloat m_fTimer;
	static bool	m_bKeys[1024];

public:
	CameraProject();
	~CameraProject();
	virtual void InitWindow(vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = true);

	virtual void Update(GLdouble a_dDeltaTime);

	virtual void Draw();

	virtual void CleanUpWin();
	virtual mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot);
	void MoveCamera(float a_fDeltaTime);

	static void key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode);
	static void scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY);
	static void mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY);

	//mat4 BuildOrbitMatrix(mat4 a_oParentRot, float a_fRad, float a_fOrbitRot);
};
#endif //!_PROJECTCAMERA_H_