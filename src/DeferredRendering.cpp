#include "DeferredRendering.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "tiny_obj_loader.h"
#include "Gizmos.h"


DeferredRendering::DeferredRendering()
{

}

DeferredRendering::~DeferredRendering()
{

}

void DeferredRendering::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	LoadShader();

	static const char* const _objNames[] =
	{
		"./models/stanford/dragon.obj",
		"./models/stanford/buddha.obj",
		"./models/stanford/bunny.obj",
		"./models/stanford/dragon.obj",
		"./models/stanford/buddha.obj",
	};

	LoadMesh(_objNames[1]);

	GenBuffers();
	GenLightBuffer();
	GenScreenSpaceQuad();
}

void DeferredRendering::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	vec4 _white(1);
	vec4 _black(0, 0, 0, 1);
	vec4 _red(1, 0, 0, 1);
	vec4 _green(0, 1, 0, 1);
	vec4 _blue(0, 0, 1, 1);

	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? _blue : _white);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? _red : _white);
	}

	OnKey();
	MoveCamera((float)a_dDeltaTime);
}

void DeferredRendering::CleanUpWin()
{
	Gizmos::destroy();
	glDeleteProgram(Program.m_uiGBufferProgram);
	glDeleteProgram(Program.m_uiDirLightProgram);
	glDeleteProgram(Program.m_uiPointLightProgram);
	glDeleteProgram(Program.m_uiSpotLightProgram);
	glDeleteProgram(Program.m_uiCompositeProgram);
	Application::CleanUpWin();
}

void DeferredRendering::Draw()
{
	mat4 _projView = m_oProjection * m_oView;
	vec4 _clearColour = vec4(0.0f);
	vec4 _clearNormal = vec4(0.5f);
	//Depth pass
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.G.m_uiFBO_GB);

	glClearBufferfv(GL_COLOR, 0, (float*)&_clearColour);
	glClearBufferfv(GL_COLOR, 1, (float*)&_clearColour);
	glClearBufferfv(GL_COLOR, 2, (float*)&_clearNormal);

	glUseProgram(Program.m_uiGBufferProgram);
	GLint _viewUni		= glGetUniformLocation(Program.m_uiGBufferProgram, "View");
	GLint _projViewUni	= glGetUniformLocation(Program.m_uiGBufferProgram, "View_Projection");

	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, value_ptr(m_oView));
	glUniformMatrix4fv(_projViewUni, 1, GL_FALSE, value_ptr(_projView));
	//Draw mesh
	DrawMesh(m_oObject[0]);
	//Light pass
	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.Light.m_uiFBO_Light);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(Program.m_uiDirLightProgram);
	
	int _posTexUni	= glGetUniformLocation(Program.m_uiCompositeProgram, "Position_Tex");
	int _normTexUni	= glGetUniformLocation(Program.m_uiCompositeProgram, "Normal_Tex");

	glUniform1i(_posTexUni, 0);
	glUniform1i(_normTexUni, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Buffer.G.m_uiPos_T);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Buffer.Light.m_uiFBO_TLight);

	vec3 _lightDir = vec3(0, -1, 0);
	vec4 _viewSpaceLightDir = m_oView * vec4(_lightDir, 0);
	vec3 _lightColour = vec3(0.8, 1, 0.43);

	int _LightDirUni = glGetUniformLocation(Program.m_uiCompositeProgram, "Light_Dir");
	int _LightColUni = glGetUniformLocation(Program.m_uiCompositeProgram, "Light_Colour");

	glUniform3fv(_LightDirUni, 1, (float*)&_lightDir);
	glUniform3fv(_LightColUni, 1, (float*)&_lightColour);

	glBindVertexArray(m_uiQuad_VAO.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(Program.m_uiCompositeProgram);

	int _albUni = glGetUniformLocation(Program.m_uiCompositeProgram, "Albedo_Tex");
	int _posUni = glGetUniformLocation(Program.m_uiCompositeProgram, "Light_Tex");

	glUniform1i(_albUni, 0);
	glUniform1i(_posUni, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Buffer.G.m_uiAlb_T);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Buffer.Light.m_uiFBO_TLight);

	glBindVertexArray(m_uiQuad_VAO.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Gizmos::draw(_projView);
}

void DeferredRendering::MoveCamera(float a_fDeltaTime)
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

void DeferredRendering::Use()
{

}

void DeferredRendering::GenBuffers()
{
	glGenFramebuffers(1, &Buffer.G.m_uiFBO_GB);
	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.G.m_uiFBO_GB);
	//Texture generation
	//Albedo
	glGenTextures(1, &Buffer.G.m_uiAlb_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.G.m_uiAlb_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Position Texture
	glGenTextures(1, &Buffer.G.m_uiPos_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.G.m_uiPos_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Normals Texture
	glGenTextures(1, &Buffer.G.m_uiNorm_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.G.m_uiNorm_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Depth buffer
	glGenRenderbuffers(1, &Buffer.G.m_uiFBO_D);
	glBindRenderbuffer(GL_RENDERBUFFER, Buffer.G.m_uiFBO_D);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	//Attaching tezxture to the frame Buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Buffer.G.m_uiAlb_T, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, Buffer.G.m_uiPos_T, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Buffer.G.m_uiNorm_T, NULL);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Buffer.G.m_uiFBO_D);
	//check frame Buffer
	GLenum _frameBuffStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (_frameBuffStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR: <FRAME BUFFER IS INCOMPLETE. \n");
	}
	//Setting draw Buffer
	GLenum _target[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, _target);
	//Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void DeferredRendering::GenLightBuffer()
{
	//frame Buffer Light
	glGenFramebuffers(1, &Buffer.Light.m_uiFBO_Light);
	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.Light.m_uiFBO_Light);
	//Texture
	glGenTextures(1, &Buffer.Light.m_uiFBO_TLight);
	glBindTexture(GL_TEXTURE_2D, Buffer.Light.m_uiFBO_TLight);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Attaching Texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Buffer.Light.m_uiFBO_TLight, NULL);
	//Generate draw buffers
	GLenum _lightTarget = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, &_lightTarget);
	//check frame Buffer
	GLenum _frameBuffStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (_frameBuffStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR: <LIGHT FRAME BUFFER IS INCOMPLETE. \n");
	}
	//Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void DeferredRendering::GenScreenSpaceQuad()
{
	vec2 _halfTexel = 1.0f / vec2(m_vScreenSize.x, m_vScreenSize.y) * 0.5f;

	float _vertexData[] =
	{
		-1, -1, 0, 1, _halfTexel.x, _halfTexel.y,
		1, -1, 0, 1, 1 - _halfTexel.x, _halfTexel.y,
		1, 1, 0, 1, 1 - _halfTexel.x, 1 - _halfTexel.y,
		-1, 1, 0, 1, _halfTexel.x, 1 - _halfTexel.y,
	};

	GLuint _indexData[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	m_uiQuad_VAO.m_uiIndexCount = 6;

	glGenVertexArrays(1, &m_uiQuad_VAO.m_uiVAO);
	glBindVertexArray(m_uiQuad_VAO.m_uiVAO);

	glGenBuffers(1, &m_uiQuad_VAO.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiQuad_VAO.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertexData), _vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_uiQuad_VAO.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiQuad_VAO.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indexData), _indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void DeferredRendering::RenderPass(bool a_bFromLight)
{

}

void DeferredRendering::OnKey()
{

}

void DeferredRendering::LoadShader()
{
	Program.m_uiGBufferProgram		= m_oShader.GenerateShaderProgram(GBUFFER_VERTEX_GLSL, nullptr, GBUFFER_FRAGMENT_GLSL);

	Program.m_uiCompositeProgram	= m_oShader.GenerateShaderProgram(COMPOSITE_VERTEX_GLSL, nullptr, COMPOSITE_FRAGMENT_GLSL);

	Program.m_uiDirLightProgram		= m_oShader.GenerateShaderProgram(COMPOSITE_VERTEX_GLSL, nullptr, LIGHT_FRAGMENT_GLSL);
}

void DeferredRendering::LoadMesh(const char* a_pccFileName)
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* float_count,
			_vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oObject[mesh_index].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			_shapes[shape_index].mesh.indices.size() * sizeof(unsigned int),
			_shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NULL, NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, NULL,
			(void*)(sizeof(float)* _shapes[shape_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void DeferredRendering::DrawMesh(MeshData a_oMesh)
{
	glBindVertexArray(a_oMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, a_oMesh.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}