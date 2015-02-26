#ifndef _EMITTERSH_H_
#define _EMITTERSH_H_
//lib
#include "gl_core_4_4.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

struct ParticleVertex
{
	glm::vec4 _colour;
	glm::vec4 _position;
	glm::vec4 _velocity;
	float _size;
	float _spawnTime;
	float _lifeSpan;
};

struct VertexParticle
{
	glm::vec4 Position;
	glm::vec4 Colour;
};

class EmitterSh
{
public:
	//Particle data
	ParticleVertex* m_oParticles;
	//Buffer data
	OpenGLInfo m_oBuffers;
	//Particle data
	VertexParticle* m_oVertexData;
	//Emitter data
	glm::vec4 m_vEmitterPos;
	glm::vec4 m_vStartCol;
	glm::vec4 m_vEndCol;
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
	//Buffer data
	unsigned int* m_uiIndexData;
	//Particle data
	unsigned int m_uiAliveCount;
	//Particle data
	unsigned int m_uiMaxParticle;

//public:
	EmitterSh();
	~EmitterSh();

	void InitEmitter(GLuint a_uiMaxParticle,
					 glm::vec4 a_vPos,
					 float a_fEmitterRate,
					 float a_fLifeSpanMin, float m_fLifeSpanMax,
					 float a_fSpeedMin, float a_fSpeedMax,
					 float a_fStartSize, float a_fEndSize,
					 glm::vec4 a_vStartCol, glm::vec4 a_vEndCol);

	void EmitParticles();

	void SimulateParticle(float a_fDeltaTime); //update

	void UpdateVertexData(glm::vec3 a_vEyePos, glm::vec3 a_vCamUp);

	void RenderParticle();
};
#endif //!_EMITTERSH_H_