#include "ParticleSh.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"

ParticleSh::ParticleSh() 
{

}
ParticleSh::~ParticleSh() 
{

}

void ParticleSh::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oEmitter.InitEmitter(2000, vec4(0, 0, 0, 0), 5.0f, 2.0f, 4.0f, 5.0f, 23.0f, 12.0f, 21.0f, vec4(0.2, 0.3, 0.4, 1), vec4(1, 1, 1, 1));

	m_oShader.CreateShaderProgram("./shaders/ParticleVertex.glsl", "./shaders/ParticleFragment.glsl");
}

void ParticleSh::CleanUpWin()
{
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void ParticleSh::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	Gizmos::clear();

	Gizmos::addTransform(mat4(1));

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

	m_oEmitter.SimulateParticle((float)a_dDeltaTime);

	m_oEmitter.UpdateVertexData(m_oWorld[3].xyz, m_oWorld[1].xyz);

	MoveCamera((float)a_dDeltaTime);
}

void ParticleSh::Draw()
{
	Use();
	mat4 _projview = m_oProjection * m_oView;
	Gizmos::draw(_projview);
	m_oEmitter.RenderParticle();
}
void ParticleSh::MoveCamera(float a_fDeltaTime)
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

void ParticleSh::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
}