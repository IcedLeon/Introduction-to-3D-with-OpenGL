#ifndef _FURSIMAPP_H_
#define _FURSIMAPP_H_
//Lib
#include "BaseApplication.h"
#include "ShaderProgram.h"
#include "FBXFile.h"

struct  LightProperties
{
	vec3 m_vDirection;
	vec4 m_vAmbientColour;
	vec4 m_vDiffuseColour;
	vec4 m_vSpecularColour;
};

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

#define AMBIENT_VERT "./shaders/AmbientDiffuseTexture.vert"
#define AMBIENT_FRAG "./shaders/AmbientDiffuseTexture.frag"
#define FUR_VERTEX "./shaders/Fur.vert"
#define FUR_GEOM "./shaders/Fur.geom"
#define FUR_FRAG "./shaders/Fur.frag"

class FurSim : public App::BaseApplication
{
private:
	ShaderProgram m_oProgram;
	ShaderProgram m_oFurProgram;
	vector<MeshData*> m_oMeshes;
	LightProperties m_oMyLights;
	GLuint m_uiTexture;
	GLuint m_uiFurVAO;
	GLuint m_uiTexFurCol;
	GLuint m_uiTexFurStrength;
	GLuint m_uiTigerTex;

	virtual void Render() override;

	virtual void Update(float a_fDeltaT) override;

	virtual void OnResize(GLint a_iWidth, GLint i_iHeight);

	virtual void OnKey(GLint a_iKey, GLint a_iAction) override;

	virtual void OnMouseButton(GLint a_iButton, GLint a_iAction) override;

	virtual void OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY) override;

	virtual void OnMouseWheel(GLdouble a_dPosition) override;
	
	virtual void Shutdown();

public:
	FurSim();
	virtual ~FurSim();

	virtual void Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen);

	void LoadShaders();
	void InitUniform();

	MeshData* LoadMesh(FBXFile* a_oFile);
	void DrawMesh(MeshData* a_oMesh);
};

#endif //!_FURSIMAPP_H_