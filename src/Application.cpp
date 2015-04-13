#include "Application.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

Camera	Application::m_oCamera = Camera();
TweekBar Application::m_oTweek = TweekBar();
vec3	Application::m_vScreenSize;
bool	Application::m_bKeys[1024] = { 0 };
static	GLdouble	 m_fPrevX;
static	GLdouble	 m_fPrevY;

  /////////////////////////////////
 //			TweekBar			//
/////////////////////////////////
void TweekBar::InitTweek()
{
	TwInit(TW_OPENGL_CORE, nullptr);
	printf("-- ANTTWEEKBAR INIT SUCCESSFULLY. \n");
}

void TweekBar::CreateBar(const char* a_sNewBarName)
{
	m_mpTweekBar[a_sNewBarName] = TwNewBar(a_sNewBarName);
}

TwBar* TweekBar::GetMappedBar(const char* a_sBarName)
{
	//Checks if the bar has been mapped.
	if (m_mpTweekBar.find(a_sBarName) != m_mpTweekBar.end())
	{
		return m_mpTweekBar[a_sBarName];
	}
	else
	{
		printf("ERROR: <COULD NOT FIND '%s' BAR FROM THE MAP. CHECK IF BAR HAS BEEN INIT BEFORE> \n", a_sBarName);
		return nullptr;
	}
}

void TweekBar::DrawTweek()
{
	TwDraw();
}

void TweekBar::CleanUpTweek()
{
	TwDeleteAllBars();
	TwTerminate();
}

void TweekBar::ScaleTweek(int a_iWidth, int a_iHeight)
{
	TwWindowSize(a_iWidth, a_iHeight);
}

template<typename T>
void TweekBar::AddR_WTweak(const char* a_pccDivisor, const char* a_pccName, T a_Var, const char* a_pccDefinition)
{
	TwAddVarRW(m_mpTweekBar[a_pccDivisor], a_pccName, TypeToTW<T>::value, &a_Var, a_pccDefinition);
}

template<typename T>
void TweekBar::AddR_OVar(const char* a_pccDivisor, const char* a_pccName, T a_Var, const char* a_pccDefinition)
{
	TwAddVarRO(m_mpTweekBar[a_pccDivisor], a_pccName, TypeToTW<T>::value, &a_Var, a_pccDefinition);
}

void TweekBar::AddTweakColor3f(const char* a_pccDivisor, const char* a_pccName, vec3 a_vCol, const char* a_pccDefinition)
{
	TwAddVarRW(m_mpTweekBar[a_pccDivisor], a_pccName, TW_TYPE_COLOR3F, &a_vCol, a_pccDefinition);
}

void TweekBar::AddTweakColor4f(const char* a_pccDivisor, const char* a_pccName, vec4 a_vCol, const char* a_pccDefinition)
{
	TwAddVarRW(m_mpTweekBar[a_pccDivisor], a_pccName, TW_TYPE_COLOR4F, &a_vCol, a_pccDefinition);
}

void TweekBar::AddTweakDir3f(const char* a_pccDivisor, const char* a_pccName, vec3 a_vDir, const char* a_pccDefinition)
{
	TwAddVarRW(m_mpTweekBar[a_pccDivisor], a_pccName, TW_TYPE_DIR3F, &a_vDir, a_pccDefinition);
}


  /////////////////////////////////
 //			Application			//
/////////////////////////////////
Application::Application() : m_oWin(nullptr),
							 //m_vScreenSize(vec3(0.0f, 0.0f, 0.0f)),
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
	printf("--------------------------------------------------------------------------------");
	if (!glfwInit())
	{
		printf("ERROR: <CANNOT INITIALIZE WINDOW. glfwInit() has fail initialization. %s\n");
	}
	else
	{
		if (!a_bFullScreen)
		{
			m_oWin = glfwCreateWindow((int)a_vScreenSize.x, (int)a_vScreenSize.y, a_pccWinName, nullptr, nullptr);
			m_vScreenSize = a_vScreenSize;
			printf("-- WINDOW CREATED SUCCESSFULLY. \n");
		}
		else
		{
			m_oWin = glfwCreateWindow((int)a_vScreenSize.x, (int)a_vScreenSize.y, a_pccWinName, glfwGetPrimaryMonitor(), nullptr);
			m_vScreenSize = a_vScreenSize;
			printf("-- WINDOW CREATED SUCCESSFULLY. \n");
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
			printf("--------------------------------------------------------------------------------");
			printf("-- OGL LOADED SUCCESSFULLY. \n");
			//
			glEnable(GL_DEPTH_TEST);
			printf("-- GL_DEPTH_TEST ENABLED. \n");
			//
			glDebugMessageCallback(debug_callback, NULL);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "START DEBUGGING\n");
			printf("-- GL DEBUG MESSAGE ENABLED. \n");
			glfwSetCharModsCallback(m_oWin, on_char_callback);
			printf("-- CHAR_CALLBACK ENABLED. \n");
			glfwSetKeyCallback(m_oWin, key_callback);
			printf("-- KEY_CALLBACK ENABLED. \n");
			glfwSetMouseButtonCallback(m_oWin, mouse_button_callback);
			glfwSetCursorPosCallback(m_oWin, mouse_callback);
			glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
			printf("-- MOUSE_CALLBACK ENABLED. \n");
			glfwSetScrollCallback(m_oWin, scroll_callback);
			printf("-- SCROLL_CALLBACK ENABLED. \n");
			glfwSetFramebufferSizeCallback(m_oWin, framebuffer_size_callback);
			printf("-- WINDOW_BUFFER_CALLBACK ENABLED. \n");
			//
			m_oCamera.BuildCamera(a_vCamPos);
			printf("-- CAMERA BUILT SUCCESSFULLY. \n");
			//Note: I should consider moving this function call into the appropriate application.
			m_oTweek.InitTweek();
			printf("--------------------------------------------------------------------------------");
			//Version print.
			GLint _majorVer = ogl_GetMajorVersion();
			GLint _minorVer = ogl_GetMinorVersion();
			printf("-- Successfully loaded this version of OpenGL %d.%d \n", _majorVer, _minorVer);
			printf("--------------------------------------------------------------------------------");
			//card and version supported.
			const GLubyte* _vendor		= glGetString(GL_VENDOR); //Get vendor
			const GLubyte* _renderer	= glGetString(GL_RENDERER); // get renderer string
			const GLubyte* _version		= glGetString(GL_VERSION); // version as a string
			const GLubyte* _glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
			printf("-- Vendor: %s\n", _vendor);
			printf("-- Renderer: %s\n", _renderer);
			printf("-- OpenGL Version: %s\n", _version);
			printf("-- GLSL Version: %s\n", _glslVersion);
			printf("--------------------------------------------------------------------------------");
		}
	}
}

void Application::CleanUpWin()
{
	m_oTweek.CleanUpTweek();
	glfwDestroyWindow(m_oWin);
	glfwTerminate();
}

void Application::ClearColor(vec4 a_vScreenColor)
{
	//CheckforOpenGLError(__FILE__, __LINE__);
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
	TwEventKeyGLFW(a_iKey, a_iAction);

	//printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void Application::mouse_button_callback(GLFWwindow* a_oWindow, int a_iButton, int a_iAction, int a_iMode)
{
	TwEventMouseButtonGLFW(a_iButton, a_iAction);
	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iButton] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iButton] = false;
}

void Application::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
{
	TwEventMousePosGLFW((int)a_iMouseX, (int)a_iMouseY);

	glfwGetCursorPos(a_oWindow, &a_iMouseX, &a_iMouseY);

	if (m_bKeys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS)
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
	TwEventMouseWheelGLFW((int)a_fOffsetY);
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

void Application::framebuffer_size_callback(GLFWwindow* a_oWindow, int a_iWidth, int a_iHeight)
{
	glfwGetFramebufferSize(a_oWindow, &a_iWidth, &a_iHeight);
	m_vScreenSize = vec3(a_iWidth, a_iHeight, NULL);
	TwWindowSize(a_iWidth, a_iHeight);
	glViewport(0, 0, a_iWidth, a_iHeight);
}

void Application::on_char_callback(GLFWwindow* a_oWindow, unsigned int a_uiCodepoint, int a_iMode)
{

}

void APIENTRY Application::debug_callback(GLenum a_eSource, GLenum a_eType, GLuint a_uiID, GLenum a_eSeverity, GLsizei a_siLength, const GLchar* a_pccMsg, const void* a_pcvParam)
{
	string _sourceString;

	switch (a_eSource)
	{
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_sourceString = "WindowSys";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		_sourceString = "App";
		break;
	case GL_DEBUG_SOURCE_API:
		_sourceString = "OpenGL";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_sourceString = "ShaderCompiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_sourceString = "3rdParty";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		_sourceString = "Other";
		break;
	default:
		_sourceString = "Unknown";
	}

	string _typeStr;

	switch (a_eType) {
	case GL_DEBUG_TYPE_ERROR:
		_typeStr = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_typeStr = "Deprecated";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_typeStr = "Undefined";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		_typeStr = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		_typeStr = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		_typeStr = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		_typeStr = "PushGrp";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		_typeStr = "PopGrp";
		break;
	case GL_DEBUG_TYPE_OTHER:
		_typeStr = "Other";
		break;
	default:
		_typeStr = "Unknown";
	}

	string _sevStr;

	switch (a_eSeverity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_sevStr = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		_sevStr = "MED";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		_sevStr = "LOW";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_sevStr = "NOTIFY";
		break;
	default:
		_sevStr = "UNKNOWN";
	}

	printf("%s:%s[%s](%d): %s\n", _sourceString.c_str(), _typeStr.c_str(), _sevStr.c_str(),
		a_uiID, a_pccMsg);
}

int Application::CheckforOpenGLError(const char* a_pccFile, int a_iLine) {
	//
	// Expect 1 if an OpenGL error occurred, otherwise a 0 would be returned.
	//
	GLenum _glErr;
	int    _retCode = 0;

	_glErr = glGetError();
	while (_glErr != GL_NO_ERROR)
	{
		const char* _message = "";
		switch (_glErr)
		{
		case GL_INVALID_ENUM:
			_message = "Invalid enum";
			break;
		case GL_INVALID_VALUE:
			_message = "Invalid value";
			break;
		case GL_INVALID_OPERATION:
			_message = "Invalid operation";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			_message = "Invalid framebuffer operation";
			break;
		case GL_OUT_OF_MEMORY:
			_message = "Out of memory";
			break;
		default:
			_message = "Unknown error";
		}

		printf("<ERROR>: file %s @ line %d: %s\n", a_pccFile, a_iLine, _message);
		_retCode = 1;
		_glErr = glGetError();
	}
	return _retCode;
}