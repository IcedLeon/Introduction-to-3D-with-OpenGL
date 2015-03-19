#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

#define RENDERING_VERTEX_GLSL	"./shaders/PostEffectVertex.glsl"
#define RENDERING_FRAGMENT_GLSL "./shaders/PostEffectFragment.glsl"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class PostProcess : public Application
{
private:
	ShaderCompiler	m_oShader;
	OpenGLInfo		m_oQuad;
	GLuint			m_uiTexture;
	GLuint			m_uiFBO;
	GLuint			m_uiFBO_T;
	GLuint			m_uiFBO_D;

public:
	PostProcess();
	~PostProcess();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenerateFrameBuffers();

	void GenScreenSpaceQuad();
}; 
#endif //!_POSTPROCESS_H_