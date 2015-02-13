#ifndef _ADVANCETEXTURE_H_
#define _ADVANCETEXTURE_H_

#include "Application.h"
#include "ShaderCompiler.h"
#include "Vertex.h"

class Camera;

#define VERTEX_SHD_CODE "./shaders/TextureVertex.glsl"
#define FRAGMENT_SHD_CODE "./shaders/FragmentShader.glsl"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class AdvanceTexture : public Application
{
	static Camera m_oCamera;
	ShaderCompiler m_oShader;

	mat4 m_oProjection;
	mat4 m_oView;
	mat4 m_oWorld;

	GLfloat m_fTimer;
	GLfloat m_fSinAug;
	GLfloat m_fTimeAug;
	GLfloat m_fModRGB;

	OpenGLInfo m_oQuad;

	GLuint m_uiIndexCount;

	GLuint m_uiDiffuseTexture;
	GLuint m_uiNormalTexture;
	GLuint m_uiSpecularTexture;

	static bool	m_bKeys[1024];

	vec3	ambient_light;
	vec3	light_direction;
	vec3	light_colour;
	float	specular_power;

public:
	AdvanceTexture();
	~AdvanceTexture();

	virtual void InitWindow(vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();

	virtual void MoveCamera(float a_fDeltaTime);

	static void key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode);
	static void scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY);
	static void mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY);

	void Use();

	void GenerateQuad(float a_fSize);
	void IncreaseValue();
	void LoadTexture();
};
#endif //!_ADVANCETEXTURE_H_