#include "GPUParticleProject.h"
#include <GLFW\glfw3.h>
#include "GPUEmitter.h"
#include "Camera.h"
#include "Gizmos.h"

GPUParticle::GPUParticle()
{

}
GPUParticle::~GPUParticle()
{

}

void GPUParticle::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oEmitter = new GPUEmitter();

	m_oEmitter->Init(10000, 
		vec3(0, 0, 0), 
		0, 
		2, 5, 
		0.8f, 1.0f,
		1, 0.001f, 
		vec4(1, 0, 0, 1), vec4(1, 0, 0, 1));

	m_fTimer = 0.0f;

	//m_oShader.CreateShaderProgram(GPU_PARTICLE_VERTEX_GLSL, GPU_PARTICLE_GEOMETRY_GLSL, GPU_PARTICLE_FRAGMENT_GLSL);
}

void GPUParticle::CleanUpWin()
{
	m_oEmitter->m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void GPUParticle::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	m_fTimer += (float)a_dDeltaTime;

	Gizmos::clear();

	Gizmos::addTransform(m_oWorld);

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

void GPUParticle::Draw()
{
	mat4 _projview = m_oProjection * m_oView;

	m_oEmitter->Render(m_fTimer, m_oWorld, _projview);

	Use();

	Gizmos::draw(_projview);

	glBindVertexArray(m_oEmitter->m_uiVAO[m_oEmitter->_otherBuff]);
	glDrawArrays(GL_POINTS, 0, m_oEmitter->m_uiMaxParticles);

	m_oEmitter->m_uiCurrBuffer = m_oEmitter->_otherBuff;
}
void GPUParticle::MoveCamera(float a_fDeltaTime)
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

void GPUParticle::Use()
{
	m_oEmitter->m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	//GLuint _modelUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
	//glUniformMatrix4fv(_modelUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));

	GLuint _cameraUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "CameraTransform");
	glUniformMatrix4fv(_cameraUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));

	GLint _sizeUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "sizeStart");
	glUniform1f(_sizeUni, m_oEmitter->m_fStartSize);

	_sizeUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "sizeEnd");
	glUniform1f(_sizeUni, m_oEmitter->m_fEndSize);

	GLint _colourUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "colourStart");
	glUniform4fv(_colourUni, 1, (float*)&m_oEmitter->m_vStartCol[0]);

	_colourUni = glGetUniformLocation(m_oEmitter->m_oShader.GetShaderProgram(), "colourEnd");
	glUniform4fv(_colourUni, 1, (float*)&m_oEmitter->m_vEndCol[0]);
}