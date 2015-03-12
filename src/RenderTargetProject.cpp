#include "RenderTargetProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"


RenderTargetP::RenderTargetP()
{

}

RenderTargetP::~RenderTargetP()
{

}

void RenderTargetP::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oShader.CreateShaderProgram(RENDER_VERTEX_GLSL, RENDER_FRAGMENT_GLSL);
}

void RenderTargetP::Update(GLdouble a_fDeltaTime)
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

	MoveCamera((float)a_fDeltaTime);
}

void RenderTargetP::CleanUpWin()
{
	Gizmos::destroy();
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}
void RenderTargetP::Draw()
{
	Use();
	mat4 _projView = m_oProjection * m_oView;
	Gizmos::draw(_projView);
}

void RenderTargetP::MoveCamera(float a_fDeltaTime)
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
}

void RenderTargetP::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	//GLuint _worldUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
	//glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));

	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Diffuse");
	glUniform1i(_textDiffUni, 0);
}

void RenderTargetP::GenerateBuffers()
{
	//
	glGenFramebuffers(1, &m_GLInfo.m_uiFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_GLInfo.m_uiFBO);
	//
	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);
	//
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiTexture, NULL);


	//
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}