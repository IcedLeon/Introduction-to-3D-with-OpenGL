#ifndef _CLOTHSIMULATION_H_
#define _CLOTHSIMULATION_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
using glm::ivec2;
//Macro
#define ADS_VERTEX				"./shaders/AdsVertex.glsl"
#define ADS_FRAGMENT			"./shaders/AdsFragment.glsl"
#define CLOTH_COMPUTE			"./shaders/ClothCompute.glsl"
#define CLOTH_NORMAL_COMPUTE	"./shaders/ClothNormalCompute.glsl"

class ClothSimulation : public Application
{
private:
	ShaderCompiler	m_oShader;
	//
	vec2			m_vClothSize;
	//
	ivec2			m_viParticleNum;
	//
	float			m_fSpeed;
	//
	GLuint			m_uiRenderProgram,
					m_uiComputeProgram,
					m_uiComputeNormProgram;
	//
	GLuint			m_uiReadBuff,
					m_uiNormBuff,
					m_uiElBuff,
					m_uiTCBuff,
					m_uiPosBuff[2],
					m_uiVelBuff[2];
	//
	struct
	{
		GLuint		m_uiVAO;
		GLuint		m_uiIndices;
	} ClothData;

	struct
	{
		GLint		m_iLightPos,
					m_iLightIntensity,
					m_iDiffuseReflectivity,
					m_iAmbientReflectivity,
					m_iSpecularReflectivity,
					m_iSpecularShininessFac;
	} RenderUni;

	struct
	{
		GLint		m_iRestLenghtHoriz,
					m_iRestLenghtVert,
					m_iRestLenghtDiag;
	} ComputeUni;

public:
	ClothSimulation();
	~ClothSimulation();

	virtual void InitWindow(vec3 a_vCamPos = vec3(3.0f, 2.0f, 5.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void CreateBuffer();
	void LoadShaders();
	void SetUniform();
};
#endif //!_CLOTHSIMULATION_H_