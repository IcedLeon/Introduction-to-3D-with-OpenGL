#ifndef _ADVANCETEXTURE_H_
#define _ADVANCETEXTURE_H_

#include "Application.h"
#include "ShaderCompiler.h"
#include "Vertex.h"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class AdvanceTexture : public Application
{
	ShaderCompiler m_oShader;

	OpenGLInfo m_oQuad;

	vec3	ambient_light;
	vec3	light_direction;
	vec3	light_colour;

	GLfloat	specular_power;

	GLfloat m_fTimer;

	GLuint m_uiDiffuseTexture;
	GLuint m_uiNormalTexture;
	GLuint m_uiSpecularTexture;

public:
	AdvanceTexture();
	~AdvanceTexture();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenerateQuad(float a_fSize);
	void IncreaseValue();
	void LoadTexture();
};
#endif //!_ADVANCETEXTURE_H_