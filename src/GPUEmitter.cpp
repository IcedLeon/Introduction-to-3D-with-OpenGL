#include "GPUEmitter.h"


GPUEmitter::GPUEmitter() : m_oParticle(nullptr),
m_vEmitterPos(vec3(0)),
m_vStartCol(vec4(0)),
m_vEndCol(vec4(0)),
m_fEmitterRate(NULL),
m_fEmitterTimer(NULL),
m_fLifeSpanMin(NULL),
m_fLifeSpanMax(NULL),
m_fSpeedMin(NULL),
m_fSpeedMax(NULL),
m_fStartSize(NULL),
m_fEndSize(NULL),
m_fLastDrawTime(NULL),
m_uiCurrBuffer(NULL),
m_uiUpdateShader(NULL),
m_uiMaxParticles(NULL)
{
	m_uiVAO[2] = { 0 };
	m_uiVBO[2] = { 0 };
}
GPUEmitter::~GPUEmitter()
{
	delete[] m_oParticle;
	glDeleteVertexArrays(2, m_uiVAO);
	glDeleteBuffers(2, m_uiVBO);

	glDeleteProgram(m_uiUpdateShader);
	glDeleteProgram(m_uiDrawShader);
}

void GPUEmitter::Init(unsigned int a_uiMaxParticle,
	vec3 a_vPos,
	float a_fEmitRate,
	float a_fMinLifeSpan, float a_fMaxLifeSpan,
	float a_fMinVel, float a_fMaxVel,
	float a_fStartSize, float a_fEndSize,
	vec4 a_vStartCol, vec4 a_vEndCol)
{
	m_uiMaxParticles = a_uiMaxParticle;
	m_vEmitterPos = a_vPos;
	m_fEmitterRate = a_fEmitRate;
	m_fLifeSpanMin = a_fMinLifeSpan;
	m_fLifeSpanMax = a_fMaxLifeSpan;
	m_fSpeedMin = a_fMinVel;
	m_fSpeedMax = a_fMaxVel;
	m_fStartSize = a_fStartSize;
	m_fEndSize = a_fEndSize;
	m_vStartCol = a_vStartCol;
	m_vEndCol = a_vEndCol;
	
	m_oParticle = new GPUParticleData[m_uiMaxParticles]();

	m_uiCurrBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUEmitter::Render(float a_fDeltaTime, mat4 a_oCameraTrans, mat4 a_oProj, mat4 a_oView)
{
	glUseProgram(m_uiUpdateShader);

	GLint _emitPosUni = glGetUniformLocation(m_uiUpdateShader, "emitterPos");
	glUniform3fv(_emitPosUni, 1, (float*)&m_vEmitterPos);
	
	GLint _timeUni = glGetUniformLocation(m_uiUpdateShader, "time");
	glUniform1f(_timeUni, a_fDeltaTime);

	GLint _deltaUni = glGetUniformLocation(m_uiUpdateShader, "deltaTime");
	float _t = a_fDeltaTime - m_fLastDrawTime;
	glUniform1f(_deltaUni, _t);

	GLint _maxVelUni = glGetUniformLocation(m_uiUpdateShader, "velocityMax");
	glUniform1f(_maxVelUni, m_fSpeedMax);

	GLint _minVelUni = glGetUniformLocation(m_uiUpdateShader, "velocityMin");
	glUniform1f(_minVelUni, m_fSpeedMin);

	GLuint _maxLifeUni = glGetUniformLocation(m_uiUpdateShader, "lifeMax");
	glUniform1f(_maxLifeUni, m_fLifeSpanMax);

	GLuint _mimLifeUni = glGetUniformLocation(m_uiUpdateShader, "lifeMin");
	glUniform1f(_mimLifeUni, m_fLifeSpanMin);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_uiVAO[m_uiCurrBuffer]);

	_otherBuff = (m_uiCurrBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, NULL, m_uiVBO[_otherBuff]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, NULL, m_uiMaxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, NULL, NULL);

	m_oShader.Use();

	glBindVertexArray(m_uiVAO[_otherBuff]);

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(a_oProj));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(a_oView));

	GLuint _cameraUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "CameraTransform");
	glUniformMatrix4fv(_cameraUni, 1, GL_FALSE, glm::value_ptr(a_oCameraTrans));

	GLint _sizeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "sizeStart");
	glUniform1f(_sizeUni, m_fStartSize);

	_sizeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "sizeEnd");
	glUniform1f(_sizeUni, m_fEndSize);

	GLint _colourUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "colourStart");
	glUniform4fv(_colourUni, 1, (float*)&m_vStartCol[0]);

	_colourUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "colourEnd");
	glUniform4fv(_colourUni, 1, (float*)&m_vEndCol[0]);

	glDrawArrays(GL_POINTS, 0, m_uiMaxParticles);
	glBindVertexArray(0);

	m_uiCurrBuffer = _otherBuff;
	m_fLastDrawTime = a_fDeltaTime;
}

void GPUEmitter::CreateBuffers()
{
	glGenVertexArrays(2, m_uiVAO);
	glGenBuffers(2, m_uiVBO);

	for (unsigned int buffIndx = 0; buffIndx < 2; ++buffIndx)
	{
		glBindVertexArray(m_uiVAO[buffIndx]);

		glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO[buffIndx]);
		glBufferData(GL_ARRAY_BUFFER, m_uiMaxParticles * sizeof(GPUParticleData), m_oParticle, GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleData), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticleData), (void*)(12));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleData), (void*)(24));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticleData), (void*)(28));
	}

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void GPUEmitter::CreateUpdateShader()
{
	GLuint _vertexShader = m_oShader.CreateShader(GL_VERTEX_SHADER, 
		"./shaders/particle_update_vert.glsl");

	m_uiUpdateShader = glCreateProgram();
	glAttachShader(m_uiUpdateShader, _vertexShader);

	const char* _outputs[] = { "Updated_Position", "Updated_Velocity", "Updated_SpawnTime", "Updated_LifeSpan" };

	glTransformFeedbackVaryings(m_uiUpdateShader, 4, _outputs, GL_SEPARATE_ATTRIBS);

	glLinkProgram(m_uiUpdateShader);

	glDeleteShader(_vertexShader);
}

void GPUEmitter::CreateDrawShader()
{
	m_oShader.CreateShaderProgram("./shaders/GPUParticleVertex.glsl", "./shaders/GPUParticleGeometry.glsl", "./shaders/GPUParticleFragment.glsl");
}