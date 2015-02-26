#include "glParticleRenderer.h"
#include "ParticleSystem.h"
#include "gl_core_4_4.h"
#include <assert.h>

void GLParticleRenderer::Generate(ParticleSystem* a_oSys, bool a_bUseQuads)
{
	assert(a_oSys != nullptr);

	m_oSystem = a_oSys;

	const size_t count = a_oSys->AllTheParticles();

	glGenVertexArrays(1, &m_uiVAO);
	glBindVertexArray(m_uiVAO);

	glGenBuffers(1, &m_uiBufPos);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBufPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * count, nullptr, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));
	//if (ogl_ext_ARB_vertex_attrib_binding)
	//{
	//	glBindVertexBuffer(0, m_uiBufPos, 0, sizeof(float) * 4);
	//	glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
	//	glVertexAttribBinding(0, 0);
	//}
	//else
		


	glGenBuffers(1, &m_uiBufCol);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBufCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * count, nullptr, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));
	//if (ogl_ext_ARB_vertex_attrib_binding)
	//{
	//	glBindVertexBuffer(1, m_uiBufCol, 0, sizeof(float) * 4);
	//	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
	//	glVertexAttribBinding(1, 1);
	//}
	//else
		


	//glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat)* 3);
	//glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat)* 3);

	//glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexAttribBinding(0, 0);
	//glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexAttribBinding(1, 1);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLParticleRenderer::Destroy()
{
	if (m_uiBufPos != 0)
	{
		glDeleteBuffers(1, &m_uiBufPos);
		m_uiBufPos = 0;
	}

	if (m_uiBufCol != 0)
	{
		glDeleteBuffers(1, &m_uiBufCol);
		m_uiBufCol = 0;
	}
}

void GLParticleRenderer::Update()
{
	assert(m_oSystem != nullptr);
	assert(m_uiBufPos > 0 && m_uiBufCol > 0);

	const size_t _count = m_oSystem->AllTheParticlesAlive();
	if (_count > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_uiBufPos);
		float* _ptr = (float *)(m_oSystem->GetParticle()->m_pvPosition.get());
		glBufferSubData(GL_ARRAY_BUFFER, 0, _count * sizeof(float) * 4, _ptr);

		glBindBuffer(GL_ARRAY_BUFFER, m_uiBufCol);
		_ptr = (float*)(m_oSystem->GetParticle()->m_pvColour.get());
		glBufferSubData(GL_ARRAY_BUFFER, 0, _count * sizeof(float) * 4, _ptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GLParticleRenderer::Render()
{
	glBindVertexArray(m_uiVAO);

	const size_t _count = m_oSystem->AllTheParticlesAlive();

	if (_count > 0)
	{
		glDrawArrays(GL_POINTS, 0, _count);
	}

	glBindVertexArray(0);
}