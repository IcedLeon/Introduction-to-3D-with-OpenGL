#include "CameraProject.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

Camera CameraProject::m_oCamera = Camera();
bool CameraProject::m_bKeys[1024];
static bool m_bMouse;
static GLdouble m_fPrevX;
static GLdouble m_fPrevY;

CameraProject::CameraProject() : Application()
{

}

CameraProject::~CameraProject()
{

}

void CameraProject::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vScreenSize, a_pccWinName, a_bFullScreen); 
	glViewport(0, 0, (GLsizei)a_vScreenSize.x, (GLsizei)a_vScreenSize.y);
	glfwSetKeyCallback(m_oWin, key_callback);
	glfwSetCursorPosCallback(m_oWin, mouse_callback);
	glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
	glfwSetScrollCallback(m_oWin, scroll_callback);
	Gizmos::create();

	m_oCamera.BuildCamera(vec3(0.0f, 0.0f, 3.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));
}

void CameraProject::CleanUpWin()
{
	Application::CleanUpWin();
	Gizmos::destroy();
}

void CameraProject::Update(GLdouble a_fDeltaTime)
{
	Gizmos::clear();
	Gizmos::addTransform(m_oCamera.GetWorldTransform());

	m_oView = m_oCamera.GetViewTransform();

	MoveCamera((float)a_fDeltaTime);

	m_fTimer += (float)GetDelta();

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10),
			glm::vec3(-10 + i, 0, 10),
			i == 10 ? m_vWhite : m_vBlack);
		Gizmos::addLine(vec3(-10, 0, -10 + i),
			glm::vec3(10, 0, -10 + i),
			i == 10 ? m_vWhite : m_vBlack);
	}
	//Sun
	mat4 _rotMat = rotate(m_fTimer, vec3(0, 1, 0));
	Gizmos::addSphere(_rotMat[3].xyz, 1.0f, 30, 30, m_vRed, &_rotMat);

	//for (unsigned int i = 0; i < PLANET_COUNT; ++i)
	//{

	//	mat4 _transPlan = _rotMat * BuildOrbitMatrix(3.0f + i, 4.0f, (float)m_fTimer * 1.5);
	//	mat4 _transChildPlan = _transPlan * BuildOrbitMatrix(3.0f, 4.0f, (float)m_fTimer * 1.5);

	//	Gizmos::addSphere(_transPlan[3].xyz, 0.5f, 30, 30, m_vGreen, &_transPlan);
	//	Gizmos::addSphere(_transChildPlan[3].xyz, 0.5f, 30, 30, m_vRed, &_transChildPlan);
	//}

	mat4 _transMatChild = _rotMat * translate(vec3(5, 0, 0)) * rotate(m_fTimer * 2.0f, vec3(0, 1, 0));

	mat4 _transMatChild2 = _transMatChild * translate(vec3(1, 0, 0)) * rotate(m_fTimer * 4.0f, vec3(0, 1, 0));

	mat4 _minecraftPlanet = _rotMat * translate(vec3(9, 2, 0)) * rotate(m_fTimer * 0.7f, vec3(1, 0, 1));

	mat4 _haloMatrix = _minecraftPlanet * translate(vec3(0, 0, 0)) * rotate(m_fTimer * 0.3f, vec3(0, 1, 0));


	Gizmos::addSphere(_transMatChild[3].xyz, 0.5f, 30, 30, m_vGreen, &_transMatChild);
	Gizmos::addSphere(_transMatChild2[3].xyz, 0.2f, 30, 30, m_vBlue, &_transMatChild2);

	Gizmos::addAABBFilled(_minecraftPlanet[3].xyz, vec3(0.3, 0.3, 0.3), m_vRed);

	Gizmos::addRing(_haloMatrix[3].xyz, 1.0f, 1.5f, 20, m_vRed, &_haloMatrix);
}

void CameraProject::Draw()
{
	Gizmos::draw(m_oProjection, m_oView);
}

mat4 CameraProject::BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot)
{
	mat4 _result = rotate(a_fOrbitRot, glm::vec3(0, 1, 0)) *
		translate(vec3(a_fRad, 0, 0)) *
		rotate(a_fLocalRot, vec3(0, 1, 0));

	return _result;
}

void CameraProject::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
{
	printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void CameraProject::MoveCamera(float a_fDeltaTime)
{
	// Camera controls
	if (m_bKeys[GLFW_KEY_W])
		m_oCamera.KeyboardInput(FORWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_S])
		m_oCamera.KeyboardInput(BACKWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_A])
		m_oCamera.KeyboardInput(LEFT, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_D])
		m_oCamera.KeyboardInput(RIGHT, a_fDeltaTime);
}

void CameraProject::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
{
	if (m_bMouse)
	{
		m_fPrevX = (float)a_iMouseX;
		m_fPrevY = (float)a_iMouseY;
		m_bMouse = false;
	}

	GLfloat xoffset = (float)(a_iMouseX - m_fPrevX);
	GLfloat yoffset = (float)(m_fPrevY - a_iMouseY);  // Reversed since y-coordinates go from bottom to left

	m_fPrevX = (float)a_iMouseX;
	m_fPrevY = (float)a_iMouseY;

	m_oCamera.MouseInput(xoffset, yoffset);
}

void CameraProject::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
{
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

//mat4 CameraProject::BuildOrbitMatrix(mat4 a_oParent, float a_fRad, float a_fOrbitRot)
//{
//	mat4 _result = rotate(a_fOrbitRot, glm::vec3(0, 1, 0)) *
//		translate(vec3(a_fRad, 0, 0)) *
//		rotate(a_fLocalRot, vec3(0, 1, 0));
//
//	return _result;
//}