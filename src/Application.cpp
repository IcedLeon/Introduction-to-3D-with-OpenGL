#include "Application.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

Camera	Application::m_oCamera = Camera();
bool	Application::m_bKeys[1024];
static	GLdouble	 m_fPrevX;
static	GLdouble	 m_fPrevY;

Application::Application() : m_oWin(nullptr),
							 m_vScreenSize(vec3(0.0f, 0.0f, 0.0f)),
							 m_dDeltaTime(NULL),
							 m_dCurrTime(NULL),
							 m_dPrevTime(NULL)
{

}

Application::~Application()
{

}

void Application::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	if (!glfwInit())
	{
		printf("ERROR: <CANNOT INITIALIZE WINDOW. glfwInit() has fail initialization. %s\n");
	}
	else
	{
		if (!a_bFullScreen)
		{
			m_oWin = glfwCreateWindow((int)a_vScreenSize.x, (int)a_vScreenSize.y, a_pccWinName, nullptr, nullptr);
			printf("WINDOW CREATED SUCCESSFULLY. \n");
		}
		else
		{
			m_oWin = glfwCreateWindow((int)a_vScreenSize.x, (int)a_vScreenSize.y, a_pccWinName, glfwGetPrimaryMonitor(), nullptr);
			printf("WINDOW CREATED SUCCESSFULLY. \n");
		}
	}
	if (!m_oWin)
	{
		glfwTerminate();
		printf("ERROR: <Window could not be intialize, glfwCreateWindow has fail.> %s\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		glfwMakeContextCurrent(m_oWin);

		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			glfwTerminate();
			printf("ERROR: <ogl_LoadFunctions has fail initialization.> %s\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("OGL LOADED SUCCESSFULLY. \n");
			//
			glEnable(GL_DEPTH_TEST);
			printf("GL_DEPTH_TEST ENABLED. \n");
			//
			glfwSetKeyCallback(m_oWin, key_callback);
			printf("KEY_CALLBACK ENABLED. \n");
			glfwSetCursorPosCallback(m_oWin, mouse_callback);
			glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
			printf("MOUSE_CALLBACK ENABLED. \n");
			glfwSetScrollCallback(m_oWin, scroll_callback);
			printf("SCROLL_CALLBACK ENABLED. \n");
			glfwSetFramebufferSizeCallback(m_oWin, framebuffer_size_callback);
			printf("WINDOW_BUFFER_CALLBACK ENABLED. \n");
			//Version print.
			GLint _majorVer = ogl_GetMajorVersion();
			GLint _minorVer = ogl_GetMinorVersion();
			printf("Successfully loaded this version of OpenGL %d.%d \n", _majorVer, _minorVer);
			//card and version supported.
			const GLubyte* _renderer = glGetString(GL_RENDERER); // get renderer string
			const GLubyte* _version = glGetString(GL_VERSION); // version as a string
			printf("Renderer: %s\n", _renderer);
			printf("OpenGL version supported %s\n", _version);
			//
			m_oCamera.BuildCamera(a_vCamPos);
			printf("CAMERA BUILT SUCCESSFULLY. \n");
		}
	}
}

void Application::CleanUpWin()
{
	glfwDestroyWindow(m_oWin);
	glfwTerminate();
}

void Application::ClearColor(vec4 a_vScreenColor)
{
	glClearColor(a_vScreenColor.x, a_vScreenColor.y, a_vScreenColor.z, a_vScreenColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Application::IsOpen() const
{
	if (!glfwWindowShouldClose(m_oWin))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Application::SwapBuffer()
{
	glfwSwapBuffers(m_oWin); 
	glfwSwapInterval(1);
	glfwPollEvents();
}

GLdouble Application::GetDelta()
{
	return m_dDeltaTime;
}

void Application::StartFrame()
{
	m_dCurrTime = glfwGetTime();
	m_dDeltaTime = m_dCurrTime - m_dPrevTime;
}

void Application::EndFrame()
{
	m_dPrevTime = m_dCurrTime;
}

void Application::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
{
	printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void Application::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
{
	if (glfwGetMouseButton(a_oWindow, 0) == GLFW_PRESS)
	{

		GLfloat _xOffset = (float)(a_iMouseX - m_fPrevX);
		GLfloat _yOffset = (float)(m_fPrevY - a_iMouseY);  // Reversed since y-coordinates go from bottom to left
		m_oCamera.MouseInput(_xOffset, _yOffset);
	}
	m_fPrevX = (float)a_iMouseX;
	m_fPrevY = (float)a_iMouseY;

}

void Application::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
{
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

void Application::framebuffer_size_callback(GLFWwindow* a_oWindow, int a_iWidth, int a_iHeight)
{
	glfwGetFramebufferSize(a_oWindow, &a_iWidth, &a_iHeight);
	glViewport(0, 0, a_iWidth, a_iHeight);
}