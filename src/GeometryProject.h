#ifndef _GEOMETRYPROJECT_H_
#define _GEOMETRYPROJECT_H_
//Lib
#include "Application.h"
#include "Vertex.h"
#include "Gizmos.h"

class Camera;

class GeometryProject : public Application
{
private:
	static Camera m_oCamera;
	mat4 m_oProjection;
	mat4 m_oView;
	GLfloat m_fTimer;
	GLuint m_uiShaderProgram;
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
	static bool	m_bKeys[1024];

public:
	virtual void InitWindow(vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);	
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot);

	void MoveCamera(float a_fDeltaTime);

	static void key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode);
	static void scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY);
	static void mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY);

	void GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void CreateShaderProgram();
	void Use();
};
#endif //!_GEOMETRYPROJECT_H_