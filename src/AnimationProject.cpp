#include "AnimationProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"

Animation::Animation() : m_vAmbientLight(NULL),
						 m_vLightDirection(NULL),
						 m_vLightColour(NULL),
						 m_fSpecularPower(NULL),
						 m_fTimer(NULL)
{

}

Animation::~Animation()
{

}

void Animation::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oShader.CreateShaderProgram(TEXTURE_VERTEX_GLSL, TEXTURE_FRAGMENT_GLSL);

	m_vAmbientLight = vec3(0.2f);
	m_vLightDirection = vec3(0.0f, -1.0f, 0.0f);
	m_vLightColour = vec3(1.0f);
	m_fSpecularPower = 15.0f;
	
	CreateFrames();
}

void Animation::CleanUpWin()
{
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void Animation::Update(GLdouble a_fDeltaTime)
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

	m_fTimer += (float)a_fDeltaTime;

	float _sinFreq = sinf(m_fTimer) * 0.5f + 0.5f;

	m_mHipBones = EvaluateKeyFrames(m_oHipFrames[0], m_oHipFrames[1], _sinFreq);

	m_mKneeBones = EvaluateKeyFrames(m_oKneeFrames[0], m_oKneeFrames[1], _sinFreq);
	
	m_mAnkleBones = EvaluateKeyFrames(m_oAnkleFrames[0], m_oAnkleFrames[1], _sinFreq);

	mat4 _worldHip = m_mHipBones;
	
	mat4 _worldKnee = m_mHipBones* m_mKneeBones;

	mat4 _worldAnkle = _worldKnee * m_mAnkleBones;

	vec3 _posHip = _worldHip[3].xyz;

	vec3 _posKnee = _worldKnee[3].xyz;

	vec3 _posAnkle = _worldAnkle[3].xyz;

	Gizmos::addSphere(_posHip, 0.2f, 20, 20, vec4(1, 1, 0, 1), &_worldHip);

	Gizmos::addSphere(_posKnee, 0.2f, 20, 20, vec4(1, 1, 0, 1), &_worldKnee);

	Gizmos::addSphere(_posAnkle, 0.2f, 20, 20, vec4(1, 1, 0, 1), &_worldAnkle);

	Gizmos::addLine(_posHip, _posKnee, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(_posKnee, _posAnkle, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	
	MoveCamera((float)a_fDeltaTime);
}

void Animation::Draw()
{
	Use();
	mat4 _projView = m_oProjection * m_oView;
	Gizmos::draw(_projView);
}

void Animation::MoveCamera(float a_fDeltaTime)
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

void Animation::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	GLuint _ambientUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "ambient_light");
	glUniform3fv(_ambientUni, 1, (float*)&m_vAmbientLight);

	GLuint _lightDirUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_direction");
	glUniform3fv(_lightDirUni, 1, (float*)&m_vLightDirection);

	GLuint _lightColUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_colour");
	glUniform3fv(_lightColUni, 1, (float*)&m_vLightColour);

	GLuint _eyeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "eye_pos");
	glUniform3fv(_eyeUni, 1, (float*)&m_oWorld[3].xyz);

	GLuint _powerUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_power");
	glUniform1f(_powerUni, m_fSpecularPower);

	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "diffuse_tex");
	glUniform1i(_textDiffUni, 0);
	GLuint _textNormalUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "normal_tex");
	glUniform1i(_textNormalUni, 1);
	GLuint _textSpecUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_tex");
	glUniform1i(_textSpecUni, 2);

	//GLuint _modRGBUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "modColor");
	//glUniform1f(_modRGBUni, m_fModRGB);

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//glBindVertexArray(m_oQuad.m_uiVAO);
	//glDrawElements(GL_TRIANGLES, m_oQuad.m_uiIndexCount, GL_UNSIGNED_INT, 0);
}

mat4 Animation::EvaluateKeyFrames(KeyFrame a_oStart, KeyFrame a_oEnd, float a_fDeltaTime)
{
	vec3 _pos = glm::mix(a_oStart.Position, a_oEnd.Position, a_fDeltaTime);

	quat _rot = glm::slerp(a_oStart.Rotation, a_oEnd.Rotation, a_fDeltaTime);

	mat4 _outputMatrix = glm::translate(_pos) * glm::toMat4(_rot);

	return _outputMatrix;
}

void Animation::CreateFrames()
{
	//Hip frames
	m_oHipFrames[0].Position = vec3(0.0f, 5.0f, 0.0f);
	m_oHipFrames[0].Rotation = quat(vec3(-1.0f, 0.0f, 0.0f));
	//Knee frames
	m_oKneeFrames[0].Position = vec3(0.0f, -2.5f, 0.0f);
	m_oKneeFrames[0].Rotation = quat(vec3(-1.0f, 0.0f, 0.0f));
	//Ankle frames
	m_oAnkleFrames[0].Position = vec3(0.0f, -2.5f, 0.0f);
	m_oAnkleFrames[0].Rotation = quat(vec3(-1.0f, 0.0f, 0.0f));
	//Hip frames
	m_oHipFrames[1].Position = vec3(0.0f, 5.0f, 0.0f);
	m_oHipFrames[1].Rotation = quat(vec3(1.0f, 0.0f, 0.0f));
	//Knee frames
	m_oKneeFrames[1].Position = vec3(0.0f, -2.5f, 0.0f);
	m_oKneeFrames[1].Rotation = quat(vec3(0.0f, 0.0f, 0.0f));
	//Ankle frames
	m_oAnkleFrames[1].Position = vec3(0.0f, -2.5f, 0.0f);
	m_oAnkleFrames[1].Rotation = quat(vec3(0.0f, 0.0f, 0.0f));
}