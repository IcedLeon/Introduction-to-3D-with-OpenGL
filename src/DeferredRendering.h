#ifndef _DEFERREDRENDERING_H_
#define _DEFERREDRENDERING_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include <vector>
//Using
using std::vector;

#define GBUFFER_VERTEX_GLSL "./shaders/GBufferVertex.glsl"
#define GBUFFER_FRAGMENT_GLSL "./shaders/GBufferFragment.glsl"
#define COMPOSITE_VERTEX_GLSL "./shaders/CompositeVertex.glsl"
#define COMPOSITE_FRAGMENT_GLSL "./shaders/CompositeFragment.glsl"
#define LIGHT_FRAGMENT_GLSL "./shaders/DirectionalLightFragment.glsl"

struct MeshData
{
	mat4   m_mModelTrans;
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class DeferredRendering : public Application
{
private:
	//
	ShaderCompiler	 m_oShader;
	vector<MeshData> m_oObject;
	//
	//struct
	//{
	//	struct
	//	{
	//		GLint m_iFocalDist;
	//		GLint m_iFocalDepth;
	//	} DOF;
	//	struct
	//	{
	//		GLint m_iMV;
	//		GLint m_iProj;
	//		GLint m_iFullShading;
	//		GLint m_iDiffuseAlb;
	//	} View;
	//} Uniforms;
	//
	struct
	{
		struct
		{
			GLuint m_uiFBO_D; // g buffer depth frame buffer
			GLuint m_uiFBO_GB; //g buffer frame buffer
			GLuint m_uiAlb_T; //Albedo(colour) frame buffer
			GLuint m_uiPos_T; //Position fbo texture
			GLuint m_uiNorm_T; //Normal fbo texture
		} G;
		struct
		{
			GLuint m_uiFBO_Light;
			GLuint m_uiFBO_TLight;
		} Light;
	} Buffer;

	MeshData m_uiQuad_VAO;
	struct
	{
		GLuint m_uiGBufferProgram;
		GLuint m_uiDirLightProgram;
		GLuint m_uiPointLightProgram;
		GLuint m_uiSpotLightProgram;
		GLuint m_uiCompositeProgram;
	} Program;

	bool m_bPaused;

public:
	DeferredRendering();
	~DeferredRendering();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenBuffers();
	void GenLightBuffer();
	void GenScreenSpaceQuad();
	void RenderPass(bool a_bFromLight);
	void OnKey();
	void LoadShader();

	void LoadMesh(const char* a_pccFileName);
	void DrawMesh(MeshData a_oMesh);
};
#endif //!_DEFERREDRENDERING_H_