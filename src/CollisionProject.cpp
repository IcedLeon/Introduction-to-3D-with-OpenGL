#include "CollisionProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"
#include "BoundingBoxes.h"

CollisionP::CollisionP()
{
	m_fTimer = NULL;
}
CollisionP::~CollisionP()
{

}

void CollisionP::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oShader.CreateShaderProgram("./shaders/ParticleVertex.glsl", "./shaders/ParticleFragment.glsl");
}

void CollisionP::CleanUpWin()
{
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void CollisionP::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	m_fTimer += (float)a_dDeltaTime;

	Gizmos::clear();

	Gizmos::addTransform(mat4(1));

	BoundingSphere _collSphere;

	_collSphere.m_vCentre = vec3(0, cosf(m_fTimer) + 1, 0);
	_collSphere.m_fRadius = 0.5f;

	vec4 _planes[6];
	//HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

	vec4 _white(1);
	vec4 _black(0, 0, 0, 1);
	vec4 _red(1, 0, 0, 1);
	vec4 _green(0, 1, 0, 1);
	vec4 _blue(0, 0, 1, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? _blue : _black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? _red : _black);
	}

	MoveCamera((float)a_dDeltaTime);
}

void CollisionP::Draw()
{
	Use();
	mat4 _projview = m_oProjection * m_oView;
	Gizmos::draw(_projview);
}

void CollisionP::MoveCamera(float a_fDeltaTime)
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

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (m_bKeys[GLFW_KEY_C])
		glEnable(GL_CULL_FACE);
	if (m_bKeys[GLFW_KEY_V])
		glDisable(GL_CULL_FACE);
}

void CollisionP::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
}


void CollisionP::GetFrustumPlanes(const mat4& a_mTransform, vec4* a_vPlanes)
{
	//right										
	a_vPlanes[0] = vec4(a_mTransform[0][3] - a_mTransform[0][0],
		a_mTransform[1][3] - a_mTransform[1][0],
		a_mTransform[2][3] - a_mTransform[2][0],
		a_mTransform[3][3] - a_mTransform[3][0]);
	//left
	a_vPlanes[1] = vec4(a_mTransform[0][3] + a_mTransform[0][0],
		a_mTransform[1][3] + a_mTransform[1][0],
		a_mTransform[2][3] + a_mTransform[2][0],
		a_mTransform[3][3] + a_mTransform[3][0]);
	//top
	a_vPlanes[3] = vec4(a_mTransform[0][3] - a_mTransform[0][1],
		a_mTransform[1][3] - a_mTransform[1][1],
		a_mTransform[2][3] - a_mTransform[2][1],
		a_mTransform[3][3] - a_mTransform[3][1]);
	//bottom
	a_vPlanes[3] = vec4(a_mTransform[0][3] + a_mTransform[0][1],
		a_mTransform[1][3] + a_mTransform[1][1],
		a_mTransform[2][3] + a_mTransform[2][1],
		a_mTransform[3][3] + a_mTransform[3][1]);
	//far
	a_vPlanes[3] = vec4(a_mTransform[0][3] - a_mTransform[0][2],
		a_mTransform[1][3] - a_mTransform[1][2],
		a_mTransform[2][3] - a_mTransform[2][2],
		a_mTransform[3][3] - a_mTransform[3][2]);

	//near
	a_vPlanes[3] = vec4(a_mTransform[0][3] + a_mTransform[0][2],
		a_mTransform[1][3] + a_mTransform[1][2],
		a_mTransform[2][3] + a_mTransform[2][2],
		a_mTransform[3][3] + a_mTransform[3][2]);

	//normalize them all
	for (int i = 0; i < 6; i++)
		a_vPlanes[i] = glm::normalize(a_vPlanes[i]);
}