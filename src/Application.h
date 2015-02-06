#ifndef _APPLICATION_H_
#define _APPLICATION_H_
//Lib
//Window and GL
#include "gl_core_4_4.h"
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
using glm::perspective;

#define WIN_NAME_PREFIX "OPENGL TEST"

struct GLFWwindow;

class Application
{
public:
	Application();
	virtual ~Application();

	virtual void InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();

	void ClearColor(vec4 a_vScreenColor);
	bool IsOpen() const;
	void SwapBuffer();

	GLdouble GetDelta();
	void StartFrame();
	void EndFrame();

protected:
	GLFWwindow* m_oWin;
	vec3 m_vScreenSize;
	GLdouble m_dDeltaTime;
	GLdouble m_dCurrTime;
	GLdouble m_dPrevTime;
};
#endif //!_APPLICATION_H_