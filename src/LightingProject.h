#ifndef _LIGHTINGPROJECT_H_
#define _LIGHTINGPROJECT_H_
//Lib
#include "Application.h"
#include "Vertex.h"
#include "ShaderCompiler.h"
#include "tiny_obj_loader.h"
#include <vector>

using std::vector;

class Camera;

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class Lighting : public Application
{
private:
	ShaderCompiler m_oShader;

	vector<OpenGLInfo>			m_gl_info;
	vector<tinyobj::shape_t>	m_oShapes;
	vector<tinyobj::material_t> m_oMaterials;

	vec3 m_vLightColour;
	//
	vec3 m_vLightDir;
	//
	vec3 m_vAmbientLight;
	//
	vec3 m_vMaterialColour;
	//Light colour modifier
	GLfloat	m_fLighColourX,
			m_fLighColourY,
			m_fLighColourZ;
	//Ligh dir modifier.
	GLfloat	m_fDirX,
			m_fDirY,
			m_fDirZ;
	//Ambient augment
	GLfloat m_fAmbientMod;
	//Material colour modifier
	GLfloat m_fMaterialX,
			m_fMaterialY,
			m_fMaterialZ;
	//Specular power
	GLfloat m_fSpecularPwr;
	//Timers
	GLfloat m_fDirDT;
	GLfloat m_fColDT;

	//static bool	m_bKeys[1024];
	bool m_bRandLight;
	bool m_bRandLightColour;
	bool m_bRandMaterial;

public:
	Lighting();
	~Lighting();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void Use();

	void MoveCamera(float a_fDeltaTime);	
	void GrabUniformLoc();
	void LoadModel(const char* a_pccFileName);
	void CreateOpenGLBuffers(vector<tinyobj::shape_t>& shapes);
	void CleanupOpenGLBuffers();	
	void ReloadShader();
	void OpenGLOptionSwitch();
	void UpdateLightingInput();
};
#endif //!_LIGHTINGPROJECT_H_