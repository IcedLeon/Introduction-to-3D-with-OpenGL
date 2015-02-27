#ifndef _GPUEMITTER_H_
#define _GPUEMITTER_H_
//Lib
#include "gl_core_4_4.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "ShaderCompiler.h"
//Using macro
using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;
//Struct
//struct OpenGLInfo
//{
//	GLuint m_uiVAO;
//	GLuint m_uiVBO;
//	GLuint m_uiIBO;
//	GLuint m_uiIndexCount;
//};

struct GPUParticleData
{
	vec3	s_position;
	vec3	s_velocity;
	GLfloat s_spawntime;
	GLfloat s_lifespan;

	GPUParticleData() : s_spawntime(1),
						s_lifespan(NULL) { }
};

class GPUEmitter
{
public:
	GPUParticleData* m_oParticle;
	ShaderCompiler m_oShader;
	//Emitter data
	vec3 m_vEmitterPos;
	//
	vec4 m_vStartCol;
	vec4 m_vEndCol;
	//Emitter data
	float m_fEmitterRate;
	//Emitter data
	float m_fEmitterTimer;
	//Emitter data
	float m_fLifeSpanMin;
	float m_fLifeSpanMax;

	float m_fSpeedMin;
	float m_fSpeedMax;

	float m_fStartSize;
	float m_fEndSize;

	float m_fLastDrawTime;

	GLuint m_uiCurrBuffer;
	GLuint m_uiVAO[2];
	GLuint m_uiVBO[2];
	GLuint _otherBuff;

	GLuint m_uiUpdateShader;
	GLuint m_uiDrawShader;
	unsigned int m_uiMaxParticles;
public:
	GPUEmitter();
	~GPUEmitter();

	void Init(unsigned int a_uiMaxParticle,
		vec3 a_vPos,
		float a_fEmitRate,
		float a_fMinLifeSpan, float a_fMaxLifeSpan,
		float a_fMinVel, float a_fMaxVel,
		float a_fStartSize, float a_fEndSize,
		vec4 a_vStartCol, vec4 a_vEndCol);

	void Render(float a_fDeltaTime, mat4 a_oCameraTrans, mat4 a_oProjView);

	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();
};
#endif //!_GPUEMITTER_H_