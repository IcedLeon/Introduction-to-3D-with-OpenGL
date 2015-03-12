#ifndef _COLLISIONPROJECT_H_
#define _COLLISIONPROJECT_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include <vector>

using std::vector;

struct AABB
{
	vec3 m_vMin;
	vec3 m_vMax;
};

struct MeshObj
{
	mat4	m_mTransform;
	AABB	m_oAABB;
	GLuint	m_uiVBO;
	GLuint	m_uiVAO;
	GLuint	m_uiIBO;
	GLuint	m_uiIndexCount;
};

class CollisionP : public Application
{
private:
	ShaderCompiler	m_oShader;
	vector<MeshObj> m_oMesh;
	vec3			m_vAmbientLight;
	vec3			m_vLightDir;
	vec3			m_vLightColour;
	vec3			m_vMaterialColour;
	float			m_fSpecularPower;
	float			m_fTimer;

public:
	CollisionP();
	~CollisionP();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();
	//Important the plane array needs to be 6 element big!!!!
	//void GetFrustumPlanes(const mat4& a_mTransform, vec4* a_vPlanes);

	void LoadMesh(const char* a_pccFileName);
	void DrawMesh(MeshObj a_oMesh);

	//void DrawFrustum(vec4 a_vPlane);
	//
	//void DrawAABB(AABB a_oCollisionBox, mat4 a_mTransform);
};
#endif //!_COLLISIONPROJECT_H_