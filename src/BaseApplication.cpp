#include "BaseApplication.h"
#include <GLFW\glfw3.h>

namespace App
{

	BaseApplication* App::BaseApplication::m_oApp = nullptr;
	bool App::BaseApplication::m_bKeys[MAX_KEYS] = { 0 };
	static GLdouble m_fPrevX = NULL;
	static GLdouble m_fPrevY = NULL;

	BaseApplication::BaseApplication()
	{
		DATA.m_oWin						= nullptr;
		DATA.m_oCurrCamera				= nullptr;
		DATA.TRANSFORM.m_mProjection	= M_IDENTITY;
		DATA.TRANSFORM.m_mView			= M_IDENTITY;
		DATA.TRANSFORM.m_mWorld			= M_IDENTITY;
		DATA.TIME.m_dDeltaT				= NULL;
		DATA.TIME.m_dCurrT				= NULL;
		DATA.TIME.m_dPrevT				= NULL;
		APPINFO.m_pccTitle				= nullptr;
		APPINFO.m_viWinSize				= I_V_2_ZERO;
		APPINFO.m_viGLVersion			= I_V_2_ZERO;
		APPINFO.m_iSample				= NULL;
	}

	BaseApplication::BaseApplication(const BaseApplication& a_Rhs)
	{
		this->DATA.m_oWin					= a_Rhs.DATA.m_oWin;
		this->DATA.m_oCurrCamera			= a_Rhs.DATA.m_oCurrCamera;
		this->DATA.m_oTotalCameras			= a_Rhs.DATA.m_oTotalCameras;
		this->DATA.TRANSFORM.m_mProjection	= a_Rhs.DATA.TRANSFORM.m_mProjection;
		this->DATA.TRANSFORM.m_mView		= a_Rhs.DATA.TRANSFORM.m_mView;
		this->DATA.TRANSFORM.m_mWorld		= a_Rhs.DATA.TRANSFORM.m_mWorld;
		this->DATA.TIME.m_dDeltaT			= a_Rhs.DATA.TIME.m_dDeltaT;
		this->DATA.TIME.m_dCurrT			= a_Rhs.DATA.TIME.m_dCurrT;
		this->DATA.TIME.m_dPrevT			= a_Rhs.DATA.TIME.m_dPrevT;
		this->APPINFO.m_pccTitle			= a_Rhs.APPINFO.m_pccTitle;
		this->APPINFO.m_viWinSize			= a_Rhs.APPINFO.m_viWinSize;
		this->APPINFO.m_viGLVersion			= a_Rhs.APPINFO.m_viGLVersion;
		this->APPINFO.m_iSample				= a_Rhs.APPINFO.m_iSample;
		this->APPINFO.Flags.m_uiAll			= a_Rhs.APPINFO.Flags.m_uiAll;
		this->APPINFO.Flags.m_uiFullScreen	= a_Rhs.APPINFO.Flags.m_uiFullScreen;
		this->APPINFO.Flags.m_uiVSync		= a_Rhs.APPINFO.Flags.m_uiVSync;
		this->APPINFO.Flags.m_uiCursor		= a_Rhs.APPINFO.Flags.m_uiCursor;
		this->APPINFO.Flags.m_uiDebug		= a_Rhs.APPINFO.Flags.m_uiDebug;
	}

	BaseApplication::~BaseApplication()
	{

	}

	void BaseApplication::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
	{
		if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
			glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

		if (a_iAction == GLFW_PRESS)
			m_bKeys[a_iKey] = true;
		else if (a_iAction == GLFW_RELEASE)
			m_bKeys[a_iKey] = false;
	}

	void BaseApplication::mouse_button_callback(GLFWwindow* a_oWindow, int a_iButton, int a_iAction, int a_iMode)
	{
		//TwEventMouseButtonGLFW(a_iButton, a_iAction);
		if (a_iAction == GLFW_PRESS)
			m_bKeys[a_iButton] = true;
		else if (a_iAction == GLFW_RELEASE)
			m_bKeys[a_iButton] = false;
	}

	void BaseApplication::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
	{
		//TwEventMousePosGLFW((int)a_iMouseX, (int)a_iMouseY);

		glfwGetCursorPos(a_oWindow, &a_iMouseX, &a_iMouseY);

		if (m_bKeys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS)
		{
			GLfloat _xOffset = (float)(a_iMouseX - m_fPrevX);
			GLfloat _yOffset = (float)(m_fPrevY - a_iMouseY);  // Reversed since y-coordinates go from bottom to left
			m_oApp->DATA.m_oCurrCamera->MouseInput(_xOffset, _yOffset);
		}
		m_fPrevX = (float)a_iMouseX;
		m_fPrevY = (float)a_iMouseY;
		m_oApp->OnMouseMove(a_iMouseX, a_iMouseY);
	}

	void BaseApplication::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
	{
		//TwEventMouseWheelGLFW((int)a_fOffsetY);
		m_oApp->DATA.m_oCurrCamera->MouseScrollZoom((float)a_fOffsetY);
	}

	void BaseApplication::framebuffer_size_callback(GLFWwindow* a_oWindow, int a_iWidth, int a_iHeight)
	{
		//TwWindowSize(a_iWidth, a_iHeight);
		m_oApp->OnResize(a_iWidth, a_iHeight);
		glfwGetFramebufferSize(a_oWindow, &a_iWidth, &a_iHeight);
		glViewport(0, 0, a_iWidth, a_iHeight);
	}

	void BaseApplication::on_char_callback(GLFWwindow* a_oWindow, unsigned int a_uiCodepoint, int a_iMode)
	{

	}

	void APIENTRY BaseApplication::debug_callback(GLenum a_eSource, GLenum a_eType, GLuint a_uiID, GLenum a_eSeverity, GLsizei a_siLength, const GLchar* a_pccMsg, const GLvoid* a_pcvParam)
	{
		//reinterpret_cast<BaseApplication*>(a_pcvParam)->OnDebugMessage(a_eSource, a_eType, a_uiID, a_eSeverity, a_siLength, a_pccMsg, a_pcvParam);
		m_oApp->GetGLError(a_eSource, a_eType, a_uiID, a_eSeverity, a_pccMsg);
	}

	void BaseApplication::OnResize(GLint a_iWidth, GLint a_iHeight)
	{
		this->m_oApp->APPINFO.m_viWinSize = ivec2(a_iWidth, a_iHeight);
		this->m_oApp->DATA.TRANSFORM.m_mProjection = this->m_oApp->DATA.m_oCurrCamera->GetProjectionTransform(this->APPINFO.m_viWinSize);
	}

	void BaseApplication::GetGLError(GLenum a_eSource, GLenum a_eType, GLuint a_uiID,
		GLenum a_eSeverity, const GLchar* a_pccMsg)
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
#define FORMAT_GL_ERROR_MSG "<OpenGL Error>: %s.\n[Source]:%s\n[Type]:%s\n[Severity]:%s\n[ID]:%d.\n"
		printf(FORMAT_GL_ERROR_MSG, a_pccMsg, _sourceString.c_str(), _typeStr.c_str(), _sevStr.c_str(),
			a_uiID);
	}

	void BaseApplication::Shutdown()
	{
		delete this->m_oApp->APPINFO.m_pccTitle;
		glfwDestroyWindow(this->DATA.m_oWin);
		glfwTerminate();
		delete this->m_oApp;
		this->m_oApp = nullptr;
	}

	void BaseApplication::CreateCamera(vec3 a_vCamPos)
	{
		static int _camID = 1; //Camera 0 is always the main one
		Camera* _newCamera = new Camera();
		_newCamera->BuildCamera(a_vCamPos);

		this->m_oApp->DATA.m_oTotalCameras[_camID] = _newCamera;

		_camID++;
	}

	void BaseApplication::SwitchCamera(int a_iCamID)
	{
		Camera* _result;

		if (this->DATA.m_oTotalCameras.count(a_iCamID) != 0)
		{
			_result = this->DATA.m_oTotalCameras.at(a_iCamID);
			this->DATA.m_oCurrCamera = _result;
		}
		else if (this->DATA.m_oTotalCameras.count(a_iCamID) == 0)
		{
			_result = this->DATA.m_oTotalCameras.at(0);
			this->DATA.m_oCurrCamera = _result;
		}
		else
		{
			printf("<WARNING>: The specif camera ID(%d) could not be found, check again. \n", a_iCamID);
		}
	}
	
	void BaseApplication::InitWindow()
	{
		printf("--------------------------------------------------------------------------------");
		if (!glfwInit())
		{
#ifdef _DEBUG
			printf("ERROR: <CANNOT INITIALIZE WINDOW. glfwInit() has fail initialization. %s\n");
#endif //!_DEBUG
			fprintf(stderr, "<ERROR>: CANNOT INITIALIZE WINDOW. glfwInit() has fail initialization.\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			ChooseResolution();
		}

		if (!this->m_oApp->DATA.m_oWin)
		{
			glfwTerminate();
#ifdef _DEBUG
			printf("<ERROR>: Window could not be intialize, glfwCreateWindow() has fail. %s\n");
#endif //!_DEBUG
			fprintf(stderr, "<ERROR>: Window could not be intialize, glfwCreateWindow() has fail. %s\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			glfwMakeContextCurrent(this->m_oApp->DATA.m_oWin);
		}
	}

	void BaseApplication::ChooseResolution()
	{
		int _resolutionTable[] = {
			640, 480,
			800, 600,
			1024, 768,
			1280, 720,
			1280, 800,
			1280, 1024,
			1366, 768,
			1400, 1050,
			1600, 1200,
			1680, 1050,
			1920, 1080
		};

#define RES_COUNT 21

#define RESIZE_WARNING "<WARNING>: Your resolution: %d x %d, has been changed because it di not conform to any standard resolution. \n"
		
		//Somehow this window hint is causing the glfwCreateWindow func to fail window creation.
		//I should investigate further once the assigment is over.
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_SAMPLES, this->m_oApp->APPINFO.m_iSample);

		GLFWmonitor* _primatyMon = glfwGetPrimaryMonitor();

		const GLFWvidmode* _modes = glfwGetVideoMode(_primatyMon);

		if (this->m_oApp->APPINFO.Flags.m_uiFullScreen)
		{
			if (this->m_oApp->APPINFO.m_viWinSize.x == 0 && this->m_oApp->APPINFO.m_viWinSize.y == 0)
			{
				unsigned int _idx = 0;
				unsigned int _nextIdx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					_nextIdx += _idx + 1;
					if ((_resolutionTable[_idx] == _modes->width) && (_resolutionTable[_nextIdx] == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[_idx], _resolutionTable[_nextIdx]);
						_found = !_found;
					}
					else if ((_resolutionTable[0] <= _modes->width) && (_resolutionTable[1] <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((_resolutionTable[20] >= _modes->width) && (_resolutionTable[21] >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					printf(RESIZE_WARNING, this->APPINFO.m_viWinSize.x, this->APPINFO.m_viWinSize.y);
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					_primatyMon,
					nullptr);
			}
			else if (this->m_oApp->APPINFO.m_viWinSize.x <= 0 && this->m_oApp->APPINFO.m_viWinSize.y > 0)
			{
				unsigned int _idx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					if ((_resolutionTable[_idx] == _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize.x = _resolutionTable[_idx];
						_found = !_found;
					}
					else if ((_resolutionTable[0] <= _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((_resolutionTable[20] >= _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
					printf(RESIZE_WARNING, this->m_oApp->APPINFO.m_viWinSize.x, this->m_oApp->APPINFO.m_viWinSize.y);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					_primatyMon,
					nullptr);
			}
			else if (this->m_oApp->APPINFO.m_viWinSize.y <= 0 && this->m_oApp->APPINFO.m_viWinSize.x > 0)
			{
				unsigned int _idx = 0;
				unsigned int _nextIdx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					_nextIdx += _idx + 1;
					if ((this->m_oApp->APPINFO.m_viWinSize.x == _modes->width) && (_resolutionTable[_nextIdx] == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize.y = _resolutionTable[_nextIdx];
						_found = !_found;
					}
					else if ((this->m_oApp->APPINFO.m_viWinSize.x <= _modes->width) && (_resolutionTable[1] <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((this->m_oApp->APPINFO.m_viWinSize.x >= _modes->width) && (_resolutionTable[21] >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					printf(RESIZE_WARNING, this->m_oApp->APPINFO.m_viWinSize.x, this->m_oApp->APPINFO.m_viWinSize.y);
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					_primatyMon,
					nullptr);

			}
			else
			{
				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					_primatyMon,
					nullptr);
			}
		}
		else
		{
			if (this->m_oApp->APPINFO.m_viWinSize.x == 0 && this->m_oApp->APPINFO.m_viWinSize.y == 0)
			{
				unsigned int _idx = 0;
				unsigned int _nextIdx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					_nextIdx += _idx + 1;
					if ((_resolutionTable[_idx] == _modes->width) && (_resolutionTable[_nextIdx] == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[_idx], _resolutionTable[_nextIdx]);
						_found = !_found;
					}
					else if ((_resolutionTable[0] <= _modes->width) && (_resolutionTable[1] <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((_resolutionTable[20] >= _modes->width) && (_resolutionTable[21] >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					printf(RESIZE_WARNING, this->m_oApp->APPINFO.m_viWinSize.x, this->m_oApp->APPINFO.m_viWinSize.y);
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					nullptr,
					nullptr);
			}
			else if (this->m_oApp->APPINFO.m_viWinSize.x <= 0 && this->m_oApp->APPINFO.m_viWinSize.y > 0)
			{
				unsigned int _idx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					if ((_resolutionTable[_idx] == _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize.x = _resolutionTable[_idx];
						_found = !_found;
					}
					else if ((_resolutionTable[0] <= _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((_resolutionTable[20] >= _modes->width) && (this->m_oApp->APPINFO.m_viWinSize.y >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
					printf(RESIZE_WARNING, this->m_oApp->APPINFO.m_viWinSize.x, this->m_oApp->APPINFO.m_viWinSize.y);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					nullptr,
					nullptr);
			}
			else if (this->m_oApp->APPINFO.m_viWinSize.y <= 0 && this->m_oApp->APPINFO.m_viWinSize.x > 0)
			{
				unsigned int _idx = 0;
				unsigned int _nextIdx = 0;
				bool _found = false;
				for (unsigned int i = 0; i < RES_COUNT; ++i)
				{
					_idx += i;
					_nextIdx += _idx + 1;
					if ((this->m_oApp->APPINFO.m_viWinSize.x == _modes->width) && (_resolutionTable[_nextIdx] == _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize.y = _resolutionTable[_nextIdx];
						_found = !_found;
					}
					else if ((this->m_oApp->APPINFO.m_viWinSize.x <= _modes->width) && (_resolutionTable[1] <= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[0], _resolutionTable[1]);
						_found = !_found;
					}
					else if ((this->m_oApp->APPINFO.m_viWinSize.x >= _modes->width) && (_resolutionTable[21] >= _modes->height))
					{
						this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[20], _resolutionTable[21]);
						_found = !_found;
					}

					if (!_found)
					{
						++_idx;
						continue;
					}
					else
						break;
				}

				if (!_found)
				{
					printf(RESIZE_WARNING, this->m_oApp->APPINFO.m_viWinSize.x, this->m_oApp->APPINFO.m_viWinSize.y);
					this->m_oApp->APPINFO.m_viWinSize = ivec2(_resolutionTable[10], _resolutionTable[11]);
				}

				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					nullptr,
					nullptr);

			}
			else
			{
				this->m_oApp->DATA.m_oWin = glfwCreateWindow(
					this->m_oApp->APPINFO.m_viWinSize.x,
					this->m_oApp->APPINFO.m_viWinSize.y,
					this->m_oApp->APPINFO.m_pccTitle,
					nullptr,
					nullptr);
			}
		}
		//delete _resolutionTable;
		printf("--------------------------------------------------------------------------------");
		printf("-- WINDOW INITIALIZED CORRECTLY. \n");
		printf("--------------------------------------------------------------------------------");
	}

	void BaseApplication::InitDependency(vec3 a_vCamPos)
	{
		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			glfwTerminate();
			printf("<ERROR>: ogl_LoadFunctions has fail initialization. \n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("--------------------------------------------------------------------------------");
			printf("-- OGL LOADED SUCCESSFULLY. \n");
			if (this->m_oApp->APPINFO.Flags.m_uiDebug)
			{ 
				glDebugMessageCallback(debug_callback, NULL);
				if (glDebugMessageCallback != NULL)
				{
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				}
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
				glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "START DEBUGGING\n");
			}
			printf("-- GL DEBUG MESSAGE ENABLED. \n");
			glfwSetCharModsCallback(this->m_oApp->DATA.m_oWin, on_char_callback);
			printf("-- CHAR_CALLBACK ENABLED. \n");
			glfwSetKeyCallback(this->m_oApp->DATA.m_oWin, key_callback);
			printf("-- KEY_CALLBACK ENABLED. \n");
			glfwSetMouseButtonCallback(this->m_oApp->DATA.m_oWin, mouse_button_callback);
			glfwSetCursorPosCallback(this->m_oApp->DATA.m_oWin, mouse_callback);
			glfwSetCursorPos(this->m_oApp->DATA.m_oWin, (double)this->m_oApp->APPINFO.m_viWinSize.x / 2.0, (double)this->m_oApp->APPINFO.m_viWinSize.y / 2.0);
			printf("-- MOUSE_CALLBACK ENABLED. \n");
			glfwSetScrollCallback(this->m_oApp->DATA.m_oWin, scroll_callback);
			printf("-- SCROLL_CALLBACK ENABLED. \n");
			glfwSetFramebufferSizeCallback(this->m_oApp->DATA.m_oWin, framebuffer_size_callback);
			printf("-- WINDOW_BUFFER_CALLBACK ENABLED. \n");
			//
			this->m_oApp->DATA.m_oCurrCamera = new Camera();
			this->m_oApp->DATA.m_oCurrCamera->BuildCamera(a_vCamPos);
			this->m_oApp->DATA.m_oTotalCameras[0] = this->m_oApp->DATA.m_oCurrCamera;
			printf("-- CAMERA BUILT SUCCESSFULLY. \n");
			//Note: I should consider moving this function call into the appropriate application.
			//m_oTweek.InitTweek();
			printf("--------------------------------------------------------------------------------");
		}
	}

	void BaseApplication::DumpGLInfo()
	{
		printf("--------------------------------------------------------------------------------");
		//Version print.
		GLint _majorVer = ogl_GetMajorVersion();
		GLint _minorVer = ogl_GetMinorVersion();
		APPINFO.m_viGLVersion = ivec2(_majorVer, _minorVer);
		printf("-- Successfully loaded this version of OpenGL %d.%d \n", this->m_oApp->APPINFO.m_viGLVersion.x, this->m_oApp->APPINFO.m_viGLVersion.y);
		printf("--------------------------------------------------------------------------------");
		//card and version supported.
		const GLubyte* _vendor = glGetString(GL_VENDOR); //Get vendor
		const GLubyte* _renderer = glGetString(GL_RENDERER); // get renderer string
		const GLubyte* _version = glGetString(GL_VERSION); // version as a string
		const GLubyte* _glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); //GLSL supported
		printf("-- Vendor: %s\n", _vendor);
		printf("-- Renderer: %s\n", _renderer);
		printf("-- OpenGL Version: %s\n", _version);
		printf("-- GLSL Version: %s\n", _glslVersion);
		printf("--------------------------------------------------------------------------------");
	}

	void BaseApplication::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
	{
		this->m_oApp								= a_oCurrApp;
		this->m_oApp->APPINFO.m_pccTitle			= a_pccWinName;
		this->m_oApp->APPINFO.m_viWinSize			= a_vScreenSize;
		this->m_oApp->APPINFO.Flags.m_uiFullScreen	= a_bFullScreen;
		this->m_oApp->APPINFO.Flags.m_uiAll			= 0;
		this->m_oApp->APPINFO.m_iSample				= 0;
		this->m_oApp->APPINFO.Flags.m_uiCursor		= 1;
#ifdef _DEBUG
		this->m_oApp->APPINFO.Flags.m_uiDebug		= 1;
#endif
		InitWindow();
		InitDependency(a_vCamPos);
		DumpGLInfo();
	}

	void BaseApplication::RunApp()
	{
		if (!this->m_oApp->APPINFO.Flags.m_uiCursor)
		{
			glfwSetInputMode(this->m_oApp->DATA.m_oWin, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		do
		{
			this->m_oApp->DATA.TIME.m_dCurrT	= glfwGetTime();
			this->m_oApp->DATA.TIME.m_dDeltaT	= this->m_oApp->DATA.TIME.m_dCurrT - this->m_oApp->DATA.TIME.m_dPrevT;

			this->m_oApp->Update((float)this->m_oApp->DATA.TIME.m_dDeltaT);
			
			this->m_oApp->Render();

			glfwSwapBuffers(this->m_oApp->DATA.m_oWin);
			glfwSwapInterval(this->m_oApp->APPINFO.Flags.m_uiVSync);
			glfwPollEvents();

			this->m_oApp->DATA.TIME.m_dPrevT = this->m_oApp->DATA.TIME.m_dCurrT;

		} while (!glfwWindowShouldClose(this->m_oApp->DATA.m_oWin));

		this->m_oApp->Shutdown();
	}

	void BaseApplication::EnableVsync(bool a_bVsyncEnable)
	{
		this->m_oApp->APPINFO.Flags.m_uiVSync = a_bVsyncEnable ? 1 : 0;
	}

} //!namespace::App