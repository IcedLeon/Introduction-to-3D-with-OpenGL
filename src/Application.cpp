#include "Application.h"

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

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
			printf("Successfully loaded this version of OpenGL %d %d \n", _majorVer, _minorVer);

			const GLubyte* _renderer = glGetString(GL_RENDERER); // get renderer string
			const GLubyte* _version = glGetString(GL_VERSION); // version as a string
			printf("Renderer: %s\n", _renderer);
			printf("OpenGL version supported %s\n", _version);
			glEnable(GL_DEPTH_TEST);
			Gizmos::create();
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
	Gizmos::destroy();
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
	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));


	m_fTimer += (float)a_dDeltaTime;

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10),
			glm::vec3(-10 + i, 0, 10),
			i == 10 ? m_vWhite : m_vBlack);
		Gizmos::addLine(vec3(-10, 0, -10 + i),
			glm::vec3(10, 0, -10 + i),
			i == 10 ? m_vWhite : m_vBlack);
	}

	mat4 _rotMat = rotate(m_fTimer, vec3(0, 1, 0));

	mat4 _transMatChild = _rotMat * translate(vec3(5, 0, 0)) * rotate(m_fTimer * 2.0f, vec3(0, 1, 0));

	mat4 _transMatChild2 = _transMatChild * translate(vec3(1, 0, 0)) * rotate(m_fTimer * 4.0f, vec3(0, 1, 0));

	mat4 _minecraftPlanet = _rotMat * translate(vec3(9, 2, 0)) * rotate(m_fTimer * 0.7f, vec3(1, 0, 1));

	mat4 _haloMatrix = _minecraftPlanet * translate(vec3(0, 0, 0)) * rotate(m_fTimer * 0.3f, vec3(0, 1, 0));

	Gizmos::addSphere(_rotMat[3].xyz, 1.0f, 30, 30, m_vRed, &_rotMat);

	Gizmos::addSphere(_transMatChild[3].xyz, 0.5f, 30, 30, m_vGreen, &_transMatChild);
	Gizmos::addSphere(_transMatChild2[3].xyz, 0.2f, 30, 30, m_vBlue, &_transMatChild2);

	Gizmos::addAABBFilled(_minecraftPlanet[3].xyz, vec3(0.3, 0.3, 0.3), m_vRed);

	Gizmos::addRing(_haloMatrix[3].xyz, 1.0f, 1.5f, 20, m_vRed, &_haloMatrix);	
}

void Application::Draw(mat4 a_oProjection, mat4 a_oView)
{
	Gizmos::draw(m_oProjection, a_oView);
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