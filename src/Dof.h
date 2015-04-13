#ifndef _DOF_H_
#define _DOF_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include "FBXFile.h"
#include <vector>
//Using
using std::vector;
//Macro
#define FBO_SIZE                2048
#define FRUSTUM_DEPTH           1000

#define DOF_VERTEX_GLSL				"./shaders/DofVertex.glsl"
#define DOF_FRAGMENT_GLSL			"./shaders/DofFragment.glsl"
#define DOF_DISPLAY_VERTEX_GLSL		"./shaders/DofDisplayVertex.glsl"
#define DOF_DISPLAY_FRAGMENT_GLSL	"./shaders/DofDisplayFragment.glsl"
#define DOF_COMPUTE_GLSL			"./shaders/GenSatCompute.glsl"

class Gizzle;

struct MeshData
{
	mat4   m_mModelTrans;
	vec4   m_vDiffuseAlbedo;
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;

	MeshData() : m_mModelTrans(mat4(0)),
				 m_vDiffuseAlbedo(vec4(0)),
				 m_uiVAO(NULL),
				 m_uiVBO(NULL),
				 m_uiIBO(NULL),
				 m_uiIndexCount(NULL)
	{ }
	MeshData(const MeshData& a_Mesh) : m_mModelTrans(a_Mesh.m_mModelTrans),
									   m_vDiffuseAlbedo(a_Mesh.m_vDiffuseAlbedo),
									   m_uiVAO(a_Mesh.m_uiVAO),
									   m_uiVBO(a_Mesh.m_uiVBO),
									   m_uiIBO(a_Mesh.m_uiIBO),
									   m_uiIndexCount(a_Mesh.m_uiIndexCount)
	{ }
};

class Dof : public Application
{
private:
	//
	FBXFile*			m_oFile;
	ShaderCompiler		m_oShader;
	vector<MeshData*>	m_oObject;
	std::thread			_newThread;
	float				m_fFocalDist;
	float				m_fFocalDepth;
	//
	struct
	{
		struct
		{
			GLint		m_iFocalDist;
			GLint		m_iFocalDepth;
		} DOF;
		struct
		{
			GLint		m_iMV;
			GLint		m_iProj;
			GLint		m_iFullShading;
			GLint		m_iDiffuseAlb;
		} View;
	} Uniforms;
	//
	struct
	{
		GLuint			m_uiFBO_D;
		GLuint			m_uiFBO_T;
		GLuint			m_uiCol_T;
		GLuint			m_uiTemp_T;
	} Buffer;

	GLuint				m_uiQuad_VAO;

	GLuint				m_uiViewProgram;
	GLuint				m_uiFilterProgram;
	GLuint				m_uiDisplayProgram;

	bool				m_bLoaded;
	bool				m_bUnlockContent;
	bool				m_bPaused;

public:
	Dof();
	~Dof();
	
	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 40.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenBuffers();
	void RenderPass();
	void OnKey();
	void LoadShader();
	void StartThreding(const char* a_pccFileName);
	void LoadFBXFile(const char* a_pccFileName);
	MeshData* LoadMesh(FBXFile* a_oFile);
	void DrawMesh(MeshData* a_oMesh);
};
#endif //!_DOF_H_