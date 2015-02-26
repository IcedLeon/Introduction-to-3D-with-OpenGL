#include "Particle.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "stb_image.h"
#include "Gizmos.h"

void Particle::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oShader.CreateShaderProgram("./shaders/ParticleEffectVertex.glsl", "./shaders/ParticleEffectFragment.glsl");

	LoadTexture();

	m_oEffects = EffectFactory::Create("tunnel");

	m_oEffects->Initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
	m_oEffects->InitializeRenderer();
	m_iCurrentEffectID = 0;
	m_oCurrentEffect = m_oEffects.get();
}

void Particle::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

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

	m_oCurrentEffect->Update(a_fDeltaTime);
	m_oCurrentEffect->CpuUpdate(a_fDeltaTime);
	m_iAllParticle = m_oCurrentEffect->AllParticles();
	m_iAliveParticle = m_oCurrentEffect->AllAliveParticles();
}

void Particle::CleanUpWin()
{
	glDeleteTextures(1, &m_uiTexture);
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void Particle::Draw()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);
	glEnable(GL_PROGRAM_POINT_SIZE);
	Use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_oCurrentEffect->Render();
	glDisable(GL_BLEND);
	mat4 _projView = m_oProjection * m_oView;
	Gizmos::draw(_projView);
}

void Particle::MoveCamera(float a_fDeltaTime)
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

void Particle::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	GLuint _modelUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
	glUniformMatrix4fv(_modelUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));

	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "tex");
	glUniform1i(_textDiffUni, 0);
}

void Particle::LoadTexture()
{
	GLint	_width,
			_height,
			_channels;

	unsigned char* _data = stbi_load("./textures/particle.png",
		&_width,
		&_height,
		&_channels,
		STBI_default);

	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);

	glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(_data);
}