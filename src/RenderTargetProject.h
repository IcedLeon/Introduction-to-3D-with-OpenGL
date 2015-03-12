#ifndef _RENDERTARGETP_H_
#define _RENDERTARGETP_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

#define RENDER_VERTEX_GLSL "./shaders/RenderingVertex.glsl"
#define RENDER_FRAGMENT_GLSL "./shaders/RenderingFragment.glsl"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiFBO;
	GLuint m_uiIndexCount;
};

class RenderTargetP : public Application
{
private:
	OpenGLInfo		m_GLInfo;
	ShaderCompiler	m_oShader;
	GLuint			m_uiTexture;

public:
	RenderTargetP();
	~RenderTargetP();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenerateBuffers();
};
#endif //!_RENDERTARGETP_H_