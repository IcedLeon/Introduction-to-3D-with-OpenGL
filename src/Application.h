#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "Constant.h"
#include <AntTweakBar.h>
//Forward declaration
struct GLFWwindow;
class Camera;

class Application
{
protected:
	//A pointer to the glfw window.
	GLFWwindow*		m_oWin;
	//The static camera object, it provvide func to move around the 3D space.
	static Camera	m_oCamera;
	//The projection matrix.
	mat4			m_oProjection;
	//The view matrix.
	mat4			m_oView;
	//World/model matrix.
	mat4			m_oWorld;
	//The screen size.
	vec3			m_vScreenSize;
	//Time related variables.
	//Delta time, the difference between one and the other update.
	GLdouble		m_dDeltaTime;
	//Current time passed.
	GLdouble		m_dCurrTime;
	//Previous time passed.
	GLdouble		m_dPrevTime;
	//Key container, keycall back instancer.
	static bool		m_bKeys[1024];
	//key_callback is a the callback function to recall the current pressed key and transform it into an iput.
	static void key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode);
	//The mouse callback. Essentially the same as the keycall but for the mouse input.
	static void mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY);
	//Same for the scroll.
	static void scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY);
	//Dinamically resize the window.
	static void framebuffer_size_callback(GLFWwindow* a_oWindow, int a_iWidth, int a_iHeight);

public:
	//Ctor & d-Ctor.
	Application();
	virtual ~Application();
	//This function re-grup all the GLFW call to initialise a new window.
	virtual void InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen);
	//This function serve the sole propouse of clealing up before we closed down our application.
	virtual void CleanUpWin();
	//This function clear up after each draw call the screen, so image do not oveloading on top of each other.
	void ClearColor(vec4 a_vScreenColor);
	//It return if the window need to be closing down. If so the app run through the clean up routine and it shut down.
	bool IsOpen() const;
	//Simply keep calling the swapping buffer and polling the event from the active window.
	void SwapBuffer();
	//It return delta time.
	GLdouble GetDelta();
	//Start frame calculate the starting frame with glfwGetTime.
	void StartFrame();
	//End frame simply set the last frame to be equal to this frame to mine the delta on top.
	void EndFrame();
	//AntTweakBar initialise funcs
	virtual void InitTweekBar();
	//Update, here we put all the function that need to be constantly keeping changing(or not) during 
	//the application is running.
	virtual void Update(GLdouble a_dDeltaTime) = NULL;
	//The draw function, the propouse of this function is to draw pretty (hopefully) thing on screen.
	//It should always contain the opengl draw function to display the current geometry.
	virtual void Draw() = NULL;
	//
	virtual void Use() = NULL;
};
#endif //!_APPLICATION_H_