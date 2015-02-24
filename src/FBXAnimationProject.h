#ifndef _FBXANIMATIONPROJECT_H_
#define _FBXANIMATIONPROJECT_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include "FBXFile.h"
#include <vector>

using std::vector;

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class FBXAnimationP : public Application
{
private:
	ShaderCompiler	m_oShader;
	FBXFile*		m_oFile;

	GLfloat			m_fTimer;
	float			m_fAugmenter;

	vector<OpenGLInfo> m_oMeshes;

public:
	FBXAnimationP();
	~FBXAnimationP();

	float m_fTestVar;

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void CreateGLMeshes(FBXFile* a_oFile);
	void EvalueteSkeleton(FBXAnimation* a_oAnimation, FBXSkeleton* a_oSkeleton, float a_fDeltaTime);
	void UpdateBones(FBXSkeleton* a_oSkeleton);
};
#endif //!_FBXANIMATIONPROJECT_H_