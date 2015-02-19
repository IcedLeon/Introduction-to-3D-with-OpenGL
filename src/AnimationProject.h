#ifndef _ANIMATIONPROJECT_H_
#define _ANIMATIONPROJECT_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

using glm::quat;

struct KeyFrame
{
	vec3 Position;
	quat Rotation;
};

class Animation : public Application
{
private:
	ShaderCompiler	m_oShader;
	
	KeyFrame		m_oHipFrames[2];
	KeyFrame		m_oKneeFrames[2];
	KeyFrame		m_oAnkleFrames[2];

	mat4			m_mHipBones;
	mat4			m_mKneeBones;
	mat4			m_mAnkleBones;

	vec3			m_vAmbientLight;
	vec3			m_vLightDirection;
	vec3			m_vLightColour;
	GLfloat			m_fSpecularPower;
	GLfloat			m_fTimer;

public:
	Animation();
	~Animation();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	mat4 EvaluateKeyFrames(KeyFrame a_oStart, KeyFrame a_oEnd, float a_fDeltaTime);
	void CreateFrames();
};
#endif //!_ANIMATIONPROJECT_H_