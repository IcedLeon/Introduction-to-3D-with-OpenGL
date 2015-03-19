#include "ShadowMap.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"
#include "tiny_obj_loader.h"

ShadowMap::ShadowMap() : Application(),
						 m_mLightViewTrans(mat4(1)),
						 m_mLightProjTrans(mat4(1)),
						 m_fTimer(NULL),
						 m_uiFBO_D(NULL),
						 m_uiFBO_T(NULL),
						 m_uiFBO_D_T(NULL),
						 m_uiVAO_Quad(NULL),
						 m_bPause(false)
{ }

ShadowMap::~ShadowMap()
{

}

void ShadowMap::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	Gizmos::create();

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	LoadShader();

	static const char* const _objNames[] =
	{
		"./models/stanford/dragon.obj",
		"./models/stanford/buddha.obj",
		"./models/stanford/bunny.obj",
		"./models/stanford/bunny.obj",
	};

	for (unsigned int i = 0; i < 4; ++i)
	{
		LoadMesh(_objNames[i]);
		m_oObject.push_back(m_oObject[i]);
	}

	glGenFramebuffers(1, &m_uiFBO_D);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO_D);

	glGenTextures(1, &m_uiFBO_T);
	glBindTexture(GL_TEXTURE_2D, m_uiFBO_T);
	glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_uiFBO_T, NULL);

	glGenTextures(1, &m_uiFBO_D_T);
	glBindTexture(GL_TEXTURE_2D, m_uiFBO_D_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiFBO_D_T, NULL);

	GLenum _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (_status != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: <FRAME BUFFER IS INCOMPLETE> \n");

	glBindTexture(GL_TEXTURE_2D, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &m_uiVAO_Quad);
	glBindVertexArray(m_uiVAO_Quad);
}

void ShadowMap::Update(GLdouble a_dDeltaTime)
{
	Gizmos::clear();
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();
	Gizmos::addTransform(mat4(1));

	static const GLfloat _zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	static float _lastTime	= 0.0f;
	static float _totalTime = 0.0f;

	if (!m_bPause)
		_totalTime += (float)(a_dDeltaTime - _lastTime);

	const float _f = _totalTime * 0.12f;

	vec3 _lightPos = vec3(_f * 1.2f, _f * 1.2f, _f * 1.2f);
	glUniform3fv(m_oUniform.View.Light_Pos, 1, (float*)&_lightPos);

	m_mLightProjTrans = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
	m_mLightViewTrans = glm::lookAt(_lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	mat4 _lightProjView = m_mLightProjTrans * m_mLightViewTrans;
	//m_oObject[0].m_mModelTrans = glm::rotate(_f * 4.5f, vec3(0, 1, 0));

	float _sinTranslation = sinf(_f * 0.37f);
	float _cosTranslation = cosf(_f * 0.37f);

	m_oObject[1].m_mModelTrans = m_oObject[0].m_mModelTrans * glm::rotate(_f, vec3(0, 1, 0)) * glm::translate(vec3(_sinTranslation * 13.0f, _cosTranslation * 17.5f, 10.0f)) * glm::scale(vec3(0.5f));
	m_oObject[2].m_mModelTrans = m_oObject[0].m_mModelTrans * glm::rotate(_f, vec3(0, 1, 0)) * glm::translate(vec3(_sinTranslation * 21.7f, _cosTranslation * 15.5f, 8.0f)) * glm::scale(vec3(0.5f));
	m_oObject[3].m_mModelTrans = m_oObject[0].m_mModelTrans * glm::rotate(_f, vec3(0, 1, 0)) * glm::translate(vec3(_sinTranslation * 32.0f, _cosTranslation * 11.0f, 4.0f)) * glm::scale(vec3(0.5f));

	OnKey();
	MoveCamera((float)a_dDeltaTime);
}

void ShadowMap::CleanUpWin()
{
	Gizmos::destroy();
	glDeleteProgram(m_uiLightProgram);
	glDeleteProgram(m_uiViewProgram);
	glDeleteProgram(m_uiShowLightDepthProgram);
	Application::CleanUpWin();
}

void ShadowMap::Draw()
{
	glEnable(GL_DEPTH_TEST);
	RenderPass(true);

	if (Mode == RENDER_DEPTH)
	{
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(m_uiVAO_Quad);
		glUseProgram(m_uiShowLightDepthProgram);
		glBindTexture(GL_TEXTURE_2D, m_uiFBO_D_T);
	}
	else
	{
		RenderPass(false);
	}
}

void ShadowMap::MoveCamera(float a_fDeltaTime)
{
	// Camera controls
	if (m_bKeys[GLFW_KEY_W])
		m_oCamera.KeyboardInput(FORWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_S])
		m_oCamera.KeyboardInput(BACKWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_A])
		m_oCamera.KeyboardInput(LEFT, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_D])
		m_oCamera.KeyboardInput(RIGHT, a_fDeltaTime);
	//Wire mode off
	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Wire mode on
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Culling on
	if (m_bKeys[GLFW_KEY_C])
		glEnable(GL_CULL_FACE);
	//Culling off
	if (m_bKeys[GLFW_KEY_V])
		glDisable(GL_CULL_FACE);
}

void ShadowMap::Use()
{

}

void ShadowMap::RenderPass(bool a_bFromLight)
{
	static const GLfloat _ones[]	= { 1.0f };
	static const GLfloat _zero[]	= { 0.0f };
	static const GLfloat _gray[]	= { 0.1f, 0.1f, 0.1f, 0.0f };

	static const mat4 _scaleBiasTrans = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
											 vec4(0.0f, 0.5f, 0.0f, 0.0f),
											 vec4(0.0f, 0.0f, 0.5f, 0.0f),
											 vec4(0.5f, 0.5f, 0.5f, 1.0f));

	mat4 _lightVPTrans		= m_mLightProjTrans * m_mLightViewTrans;
	mat4 _shadowSBPVTrans	= _scaleBiasTrans * m_mLightProjTrans * m_mLightViewTrans;

	if (a_bFromLight)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO_D);
		glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(4.0f, 4.0f);
		glUseProgram(m_uiLightProgram);
		static const GLenum _buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, _buffers);
		glClearBufferfv(GL_COLOR, NULL, _zero);
	}
	else
	{
		glViewport(0, 0, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
		glClearBufferfv(GL_COLOR, NULL, _gray);
		glUseProgram(m_uiViewProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_uiFBO_T);
		glUniformMatrix4fv(m_oUniform.View.P_Mat, 1, GL_FALSE, glm::value_ptr(m_oProjection));
		glDrawBuffer(GL_BACK);
	}

	glClearBufferfv(GL_DEPTH, NULL, _ones);
	int i;
	for (i = 0; i < 4; ++i)
	{
		mat4& _modelMat = m_oObject[i].m_mModelTrans;
		if (a_bFromLight)
		{
			mat4 _result = _lightVPTrans * _modelMat;
			glUniformMatrix4fv(m_oUniform.Light.MVP_Lht, 1, GL_FALSE, glm::value_ptr(_result));
		}
		else
		{
			mat4 _result = _shadowSBPVTrans * _modelMat;
			glUniformMatrix4fv(m_oUniform.View.Shw_Mat, 1, GL_FALSE, value_ptr(_result));
			_result = m_oView * _modelMat;
			glUniformMatrix4fv(m_oUniform.View.MV_Mat, 1, GL_FALSE, value_ptr(_result));
			glUniform1i(m_oUniform.View.Full_Shading, Mode == RENDER_FULL ? 1 : 0);
		}
		DrawMesh(m_oObject[i]);
	}

	if (a_bFromLight)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}

void ShadowMap::OnKey()
{
	if (m_bKeys[GLFW_KEY_1])
		Mode = RENDER_FULL;
	if (m_bKeys[GLFW_KEY_2])
		Mode = RENDER_LIGHT;
	if (m_bKeys[GLFW_KEY_3])
		Mode = RENDER_DEPTH;
	if (m_bKeys[GLFW_KEY_P])
		m_bPause = !m_bPause;
}

void ShadowMap::LoadShader()
{
	if (m_uiLightProgram)
		glDeleteProgram(m_uiLightProgram);

	m_uiLightProgram = m_oShader.GenerateShaderProgram(SHADOW_LIGHT_VERTEX, nullptr, SHADOW_LIGHT_FRAGMENT);

	m_oUniform.Light.MVP_Lht = glGetUniformLocation(m_uiLightProgram, "MVP");

	if (m_uiViewProgram)
		glDeleteProgram(m_uiViewProgram);

	m_uiViewProgram = m_oShader.GenerateShaderProgram(SHADOW_CAMERA_VERTEX, nullptr, SHADOW_CAMERA_FRAGMENT);

	m_oUniform.View.P_Mat			= glGetUniformLocation(m_uiViewProgram, "Proj_Matrix");
	m_oUniform.View.MV_Mat			= glGetUniformLocation(m_uiViewProgram, "MV_Matrix");
	m_oUniform.View.Shw_Mat			= glGetUniformLocation(m_uiViewProgram, "Shadow_Matrix");
	m_oUniform.View.Full_Shading	= glGetUniformLocation(m_uiViewProgram, "Full_Shading");
	m_oUniform.View.Light_Pos		= glGetUniformLocation(m_uiViewProgram, "Light_Pos");

	if (m_uiShowLightDepthProgram)
		glDeleteProgram(m_uiShowLightDepthProgram);

	m_uiShowLightDepthProgram = m_oShader.GenerateShaderProgram(SHADOW_VIEW_VERTEX, nullptr, SHADOW_VIEW_FRAGMENT);
}

void ShadowMap::LoadMesh(const char* a_pccFileName)
{
	std::vector<tinyobj::shape_t> _shapes;
	std::vector<tinyobj::material_t> _materials;

	tinyobj::LoadObj(_shapes, _materials, a_pccFileName);

	unsigned int mesh_count = m_oObject.size();

	m_oObject.resize(m_oObject.size() + _shapes.size());

	for (unsigned int shape_index = 0; shape_index < _shapes.size(); ++shape_index)
	{
		unsigned int mesh_index = mesh_count + shape_index;

		std::vector<float> _vertexData;

		unsigned int float_count = _shapes[shape_index].mesh.positions.size();
		float_count += _shapes[shape_index].mesh.normals.size();

		_vertexData.reserve(float_count);

		_vertexData.insert(_vertexData.end(),
			_shapes[shape_index].mesh.positions.begin(),
			_shapes[shape_index].mesh.positions.end());

		_vertexData.insert(_vertexData.end(),
			_shapes[shape_index].mesh.normals.begin(),
			_shapes[shape_index].mesh.normals.end());

		m_oObject[mesh_index].m_uiIndexCount =
			_shapes[shape_index].mesh.indices.size();

		glGenVertexArrays(1, &m_oObject[mesh_index].m_uiVAO);
		glGenBuffers(1, &m_oObject[mesh_index].m_uiVBO);
		glGenBuffers(1, &m_oObject[mesh_index].m_uiIBO);

		glBindVertexArray(m_oObject[mesh_index].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_oObject[mesh_index].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * float_count,
			_vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oObject[mesh_index].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			_shapes[shape_index].mesh.indices.size() * sizeof(unsigned int),
			_shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NULL, NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, NULL,
			(void*)(sizeof(float) * _shapes[shape_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ShadowMap::DrawMesh(MeshData a_oMesh)
{
	glBindVertexArray(a_oMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, a_oMesh.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}