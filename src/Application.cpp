#include "Application.h"
#include <GLFW\glfw3.h>

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

void Application::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
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
		}
		else
		{
			m_oWin = glfwCreateWindow((int)a_vScreenSize.x, (int)a_vScreenSize.y, a_pccWinName, glfwGetPrimaryMonitor(), nullptr);
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
			GLint _majorVer = ogl_GetMajorVersion();
			GLint _minorVer = ogl_GetMinorVersion();
			printf("Successfully loaded this version of OpenGL %d.%d \n", _majorVer, _minorVer);

			const GLubyte* _renderer = glGetString(GL_RENDERER); // get renderer string
			const GLubyte* _version = glGetString(GL_VERSION); // version as a string
			printf("Renderer: %s\n", _renderer);
			printf("OpenGL version supported %s\n", _version);
			glEnable(GL_DEPTH_TEST);
			glViewport(0, 0, (GLsizei)a_vScreenSize.x, (GLsizei)a_vScreenSize.y);
		}
	}
}

void Application::SwapBuffer()
{
	glfwSwapBuffers(m_oWin);
	glfwPollEvents();
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
	if (!glfwWindowShouldClose(m_oWin) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Application::Update(GLdouble a_dDeltaTime)
{

}

void Application::Draw()
{

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