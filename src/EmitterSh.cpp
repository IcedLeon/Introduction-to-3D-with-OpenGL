#include "EmitterSh.h"

EmitterSh::EmitterSh() : m_oParticles(nullptr),
						 m_oVertexData(nullptr),
						 m_vEmitterPos(glm::vec4(0)),
						 m_vStartCol(glm::vec4(0)),
						 m_vEndCol(glm::vec4(0)),
						 m_fEmitterRate(NULL),
						 m_fEmitterTimer(NULL),
						 m_fLifeSpanMin(NULL),
						 m_fLifeSpanMax(NULL),
						 m_fSpeedMin(NULL),
						 m_fSpeedMax(NULL),
						 m_fStartSize(NULL),
						 m_fEndSize(NULL),
						 m_uiIndexData(NULL),
						 m_uiAliveCount(NULL),
						 m_uiMaxParticle(NULL)
{
	m_oBuffers = {};
}

EmitterSh::~EmitterSh()
{
	delete[] m_oParticles;
	delete[] m_oVertexData;
	delete[] m_uiIndexData;

	glDeleteBuffers(1, &m_oBuffers.m_uiVAO);
	glDeleteBuffers(1, &m_oBuffers.m_uiVBO);
	glDeleteBuffers(1, &m_oBuffers.m_uiIBO);
}

void EmitterSh::InitEmitter(GLuint a_uiMaxParticle,
							glm::vec4 a_vPos,
							float a_fEmitterRate,
							float a_fLifeSpanMin, float a_fLifeSpanMax,
							float a_fSpeedMin, float a_fSpeedMax,
							float a_fStartSize, float a_fEndSize,
							glm::vec4 a_vStartCol, glm::vec4 a_vEndCol)
{
	m_uiMaxParticle = a_uiMaxParticle;
	m_uiAliveCount	= NULL;
	m_vEmitterPos	= a_vPos;
	m_fEmitterRate	= 1.0f / a_fEmitterRate;
	m_fLifeSpanMin	= a_fLifeSpanMin;
	m_fLifeSpanMax	= a_fLifeSpanMax;
	m_fSpeedMin		= a_fSpeedMin;
	m_fSpeedMax		= a_fSpeedMax;
	m_fStartSize	= a_fStartSize;
	m_fEndSize		= a_fEndSize;
	m_vStartCol		= a_vStartCol;
	m_vEndCol		= a_vEndCol;

	m_oParticles	= new ParticleVertex[m_uiMaxParticle];
	m_oVertexData	= new VertexParticle[m_uiMaxParticle * 4];
	m_uiIndexData	= new unsigned int[m_uiMaxParticle * 6];

	for (unsigned int i = 0; i < m_uiMaxParticle; ++i)
	{
		unsigned int _start = 4 * i;

		m_uiIndexData[i * 6 + 0] = _start + 0;
		m_uiIndexData[i * 6 + 1] = _start + 1;
		m_uiIndexData[i * 6 + 2] = _start + 2;

		m_uiIndexData[i * 6 + 3] = _start + 0;
		m_uiIndexData[i * 6 + 4] = _start + 2;
		m_uiIndexData[i * 6 + 5] = _start + 3;
	}

	glGenVertexArrays(1, &m_oBuffers.m_uiVAO);
	glGenBuffers(1, &m_oBuffers.m_uiVBO);
	glGenBuffers(1, &m_oBuffers.m_uiIBO);

	glBindVertexArray(m_oBuffers.m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_oBuffers.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiMaxParticle * 4 * sizeof(VertexParticle), m_oVertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBuffers.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiMaxParticle * 6 * sizeof(unsigned int), m_uiIndexData, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //col

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), (void*)sizeof(glm::vec4));

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void EmitterSh::EmitParticles()
{
	unsigned int _particleToEmit = (unsigned int)(m_fEmitterTimer / m_fEmitterRate);

	m_fEmitterTimer -= _particleToEmit * m_fEmitterRate;

	for (unsigned int i = 0; i < _particleToEmit && m_uiAliveCount < m_uiMaxParticle; ++i)
	{
		m_oParticles[m_uiAliveCount]._position		= m_vEmitterPos;
		m_oParticles[m_uiAliveCount]._spawnTime		= 0;
		m_oParticles[m_uiAliveCount]._lifeSpan		= glm::linearRand(m_fLifeSpanMin, m_fLifeSpanMax);
		m_oParticles[m_uiAliveCount]._colour		= m_vStartCol;
		float _speed								= glm::linearRand(m_fSpeedMin, m_fSpeedMax);
		m_oParticles[m_uiAliveCount]._velocity.x	= glm::sphericalRand(_speed).x;
		m_oParticles[m_uiAliveCount]._velocity.y	= glm::sphericalRand(_speed).y;
		m_oParticles[m_uiAliveCount]._velocity.z	= glm::sphericalRand(_speed).z;
		m_oParticles[m_uiAliveCount]._velocity.w	= 0;
		++m_uiAliveCount;
	}
}

void EmitterSh::SimulateParticle(float a_fDeltaTime)
{
	m_fEmitterTimer += a_fDeltaTime;
	//Clena up
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		m_oParticles[i]._spawnTime += a_fDeltaTime;
		if (m_oParticles[i]._spawnTime > m_oParticles[i]._lifeSpan)
		{
			--m_uiAliveCount;
			m_oParticles[i] = m_oParticles[m_uiAliveCount];
			--i;
		}
	}
	EmitParticles();	
	//move dead particle
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		m_oParticles[i]._position += m_oParticles[i]._velocity * a_fDeltaTime;

		float _t = m_oParticles[i]._spawnTime / m_oParticles[i]._lifeSpan;
		m_oParticles[i]._colour = glm::mix(m_vStartCol, m_vEndCol, _t);
		m_oParticles[i]._size	= glm::mix(m_fStartSize, m_fEndSize, _t);
	}
}

void EmitterSh::UpdateVertexData(glm::vec3 a_vEyePos, glm::vec3 a_vCamUp)
{
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		glm::vec4 _to = glm::vec4(a_vEyePos, 1) - m_oParticles[i]._position;

		glm::vec3 _f = glm::normalize(glm::vec3(_to.x, _to.y, _to.z));
		glm::vec3 _r = glm::cross(a_vCamUp, _f);
		glm::vec3 _u = glm::cross(_f, _r);

		m_oParticles[i]._position.w = 1;
		glm::mat4 _particleTransf(glm::vec4(_r, 0), glm::vec4(_u, 0), glm::vec4(_f, 0), m_oParticles[i]._position);

		m_oVertexData[i * 4 + 0].Position = _particleTransf * glm::vec4(-1,  1, 0, 1);
		m_oVertexData[i * 4 + 1].Position = _particleTransf * glm::vec4(-1, -1, 0, 1);
		m_oVertexData[i * 4 + 2].Position = _particleTransf * glm::vec4( 1, -1, 0, 1);
		m_oVertexData[i * 4 + 3].Position = _particleTransf * glm::vec4( 1,	 1, 0, 1);

		m_oVertexData[i * 4 + 0].Colour = m_oParticles[i]._colour;
		m_oVertexData[i * 4 + 1].Colour = m_oParticles[i]._colour;
		m_oVertexData[i * 4 + 2].Colour = m_oParticles[i]._colour;
		m_oVertexData[i * 4 + 3].Colour = m_oParticles[i]._colour;
	}
}

void EmitterSh::RenderParticle()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_oBuffers.m_uiVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_uiAliveCount * 4 * sizeof(VertexParticle), m_oVertexData);

	glBindVertexArray(m_oBuffers.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6 * m_uiAliveCount, GL_UNSIGNED_INT, NULL);
}