#ifndef _APPLICATION_H_
#define _APPLICATION_H_
//Lib
//Window and GL
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
//Math
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
//Others
#include <thread>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::lookAt;
using glm::radians;
using glm::rotation;
using glm::transpose;

#define WIN_NAME_PREFIX "OPENGL TEST"

class Application
{
public:
	Application();
	~Application();

	void InitWindow(vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	void SwapBuffer();
	void CleanUpWin();
	void ClearColor(vec4 a_vScreenColor);
	bool IsOpen() const;

	void Update(GLdouble a_dDeltaTime);

	void Draw(mat4 a_oProjection, mat4 a_oView);

	GLdouble GetDelta();
	void StartFrame();
	void EndFrame();

private:
	GLFWwindow* m_oWin;
	mat4 m_oProjection;
	vec3 m_vScreenSize;
	GLdouble m_dDeltaTime;
	GLdouble m_dCurrTime;
	GLdouble m_dPrevTime;
	GLfloat m_fTimer;
};
#endif //!_APPLICATION_H_