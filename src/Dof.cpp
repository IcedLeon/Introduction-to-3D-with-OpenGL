#include "Dof.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "tiny_obj_loader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Dof::Dof() : Application(),
			 m_fFocalDist(NULL),
			 m_fFocalDepth(NULL),
			 m_uiQuad_VAO(NULL),
			 m_bPaused(false)
{ }

Dof::~Dof()
{

}

void Dof::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
	
	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y), 2.0f, 300.0f);

	m_fFocalDist = 40.0f;
	m_fFocalDepth = 50.0f;

	LoadShader();

	static const char* const _objNames[] =
	{
		"./models/stanford/dragon.obj",
		"./models/stanford/buddha.obj",
		"./models/stanford/bunny.obj",
		"./models/stanford/dragon.obj",
		"./models/stanford/Batman.obj",
	};

	static const vec4 object_colors[] =
	{
		vec4(1.0f, 0.7f, 0.8f, 1.0f),
		vec4(0.7f, 0.8f, 1.0f, 1.0f),
		vec4(0.3f, 0.9f, 0.4f, 1.0f),
		vec4(0.6f, 0.4f, 0.9f, 1.0f),
		vec4(0.8f, 0.2f, 0.1f, 1.0f),
	};

	for (unsigned int i = 0; i < 5; ++i)
	{
		LoadMesh(_objNames[i]);
		m_oObject[i].m_vDiffuseAlbedo = object_colors[i];
		m_oObject.push_back(m_oObject[i]);
	}

	GenBuffers();
}

void Dof::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	//m_oWorld = m_oCamera.GetWorldTransform();
	//m_oView = glm::lookAt(vec3(0.0f, 0.0f, 40.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	static const GLfloat _zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	static float _lastTime = 0.0f;
	static float _totalTime = 0.0f;

	if (!m_bPaused)
		_totalTime += (float)(a_dDeltaTime - _lastTime);

	const float _f = _totalTime * 0.12f;

	m_oObject[0].m_mModelTrans = glm::translate(vec3(5.0f, 0.0f, 20.0f)) *
								 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
								 glm::translate(vec3(0.0f, -4.0f, 0.0f));

	m_oObject[1].m_mModelTrans = glm::translate(vec3(-5.0f, 0.0f, 20.0f)) *
								 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
								 glm::translate(vec3(0.0f, -4.0f, 0.0f));

	m_oObject[2].m_mModelTrans = glm::translate(vec3(-15.0f, 0.0f, 20.0f)) *
								 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
								 glm::translate(vec3(0.0f, -4.0f, 0.0f));
	
	m_oObject[3].m_mModelTrans = glm::translate(vec3(-25.0f, 0.0f, 20.0f)) *
								 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
								 glm::translate(vec3(0.0f, -4.0f, 0.0f));
	
	m_oObject[4].m_mModelTrans = glm::translate(vec3(-35.0f, 0.0f, 20.0f)) *
								 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
								 glm::translate(vec3(0.0f, -4.0f, 0.0f));
	OnKey();
	MoveCamera((float)a_dDeltaTime);
}

void Dof::CleanUpWin()
{
	glDeleteProgram(m_uiDisplayProgram);
	glDeleteProgram(m_uiFilterProgram);
	glDeleteProgram(m_uiViewProgram);
	Application::CleanUpWin();
}

void Dof::Draw()
{
	glEnable(GL_DEPTH_TEST);
	RenderPass();

	glUseProgram(m_uiFilterProgram);
	glBindImageTexture(0, Buffer.m_uiCol_T, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, Buffer.m_uiTemp_T, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute((int)m_vScreenSize.y, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, Buffer.m_uiTemp_T, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, Buffer.m_uiCol_T, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute((int)m_vScreenSize.x, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_2D, Buffer.m_uiCol_T);

	glDisable(GL_DEPTH_TEST);
	
	glUseProgram(m_uiDisplayProgram);
	glUniform1f(Uniforms.DOF.m_iFocalDist, m_fFocalDist);
	glUniform1f(Uniforms.DOF.m_iFocalDepth, m_fFocalDepth);
	glBindVertexArray(m_uiQuad_VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Dof::MoveCamera(float a_fDeltaTime)
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

void Dof::Use()
{

}

void Dof::GenBuffers()
{
	glGenFramebuffers(1, &Buffer.m_uiFBO_D);
	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.m_uiFBO_D);

	glGenTextures(1, &Buffer.m_uiFBO_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.m_uiFBO_T);
	glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &Buffer.m_uiCol_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.m_uiCol_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glGenTextures(1, &Buffer.m_uiTemp_T);
	glBindTexture(GL_TEXTURE_2D, Buffer.m_uiTemp_T);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Buffer.m_uiFBO_T, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Buffer.m_uiCol_T, 0);

	GLenum _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (_status != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: <FRAME BUFFER IS INCOMPLETE> \n");

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &m_uiQuad_VAO);
	glBindVertexArray(m_uiQuad_VAO);
}

void Dof::RenderPass()
{
	static const GLfloat _ones[]		= { 1.0f };
	static const GLfloat _zero[]		= { 0.0f };
	static const GLfloat _gray[]		= { 0.1f, 0.1f, 0.1f, 0.0f };
	static const GLenum	_attachment[]	= { GL_COLOR_ATTACHMENT0 };
	static const mat4 _scaleBiasTrans	= mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
											   vec4(0.0f, 0.5f, 0.0f, 0.0f),
											   vec4(0.0f, 0.0f, 0.5f, 0.0f),
											   vec4(0.5f, 0.5f, 0.5f, 1.0f) );

	glBindFramebuffer(GL_FRAMEBUFFER, Buffer.m_uiFBO_D);
	glDrawBuffers(1, _attachment);
	glViewport(0, 0, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
	glClearBufferfv(GL_COLOR, 0, _gray);
	glClearBufferfv(GL_DEPTH, 0, _ones);

	glUseProgram(m_uiViewProgram);
	glUniformMatrix4fv(Uniforms.View.m_iProj, 1, GL_FALSE, value_ptr(m_oProjection));

	glClearBufferfv(GL_DEPTH, 0, _ones);

	int i;
	for (i = 0; i < 4; ++i)
	{
		mat4& _modelMat = m_oObject[i].m_mModelTrans;
		glUniformMatrix4fv(Uniforms.View.m_iMV, 1, GL_FALSE, value_ptr(m_oView * _modelMat));
		glUniform3fv(Uniforms.View.m_iDiffuseAlb, 1, (float*)&m_oObject[i].m_vDiffuseAlbedo);
		DrawMesh(m_oObject[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void Dof::OnKey()
{
	if (m_bKeys[GLFW_KEY_1])
		m_fFocalDist *= 1.1f;
	if (m_bKeys[GLFW_KEY_2])
		m_fFocalDist /= 1.1f;
	if (m_bKeys[GLFW_KEY_3])
		m_fFocalDepth *= 1.1f;
	if (m_bKeys[GLFW_KEY_4])
		m_fFocalDepth /= 1.1f;
	if (m_bKeys[GLFW_KEY_P])
		m_bPaused = !m_bPaused;
	if (m_bKeys[GLFW_KEY_R])
		LoadShader();
}

GLuint ComputeShaderLoader(GLenum a_eShaderID, const GLchar* a_strShaderFileID)
{
	std::string _shaderString;
	//Try and catch safe handle shader initialization
	try
	{
		//Open file to stream
		std::ifstream _shaderFile(a_strShaderFileID);
		//Read file buffer content into stream.
		std::stringstream _shaderStream;
		_shaderStream << _shaderFile.rdbuf();
		//Close file handlers
		_shaderFile.close();
		//Convert stream into string
		_shaderString = _shaderStream.str();
	}
	catch (std::exception e)
	{
		printf("ERROR: <SHADER FILE COULD NOT BE READ SUCCESFULLY> %s\n", e);
	}
	//Reconversion to const char
	const GLchar* _shaderCode = _shaderString.c_str();
	//Time to compile the Shader
	GLuint _currShader;
	//Safe handle
	GLint _success;
	//in case the shader compaling is failing, this log will record the cause of the failure 
	//since there's no other way to tell if the shader has being compalid correclty or not.
	GLchar _infoLog[SHD_ERR_LGH];
	//Create the current Shader
	_currShader = glCreateShader(a_eShaderID);
	glShaderSource(_currShader, 1, &_shaderCode, NULL);
	glCompileShader(_currShader);
	//Check if any errors has occurred during the shader compilation
	glGetShaderiv(_currShader, GL_COMPILE_STATUS, &_success);
	//Print error in case of falliure
	if (!_success)
	{
		glGetShaderInfoLog(_currShader, SHD_ERR_LGH, NULL, _infoLog);
		printf("ERROR: <SHADER COMPILATION HAS FAILED> %s\n", _infoLog);
		delete[] & _infoLog;
	}
	return _currShader;
}

void ComputeProgram(const char* a_pccComputeID, GLuint* a_uiProgram)
{
	GLuint _result = NULL;

	if (a_pccComputeID == nullptr)
	{
		printf("ERROR: <YOUR COMPUTE SHADER: %s, COULD NOT BE FOUND. CHECK PATH OR VS PROPERTIES>", a_pccComputeID);
	}
	else
	{
		_result = ComputeShaderLoader(GL_COMPUTE_SHADER, a_pccComputeID);
	}

	*a_uiProgram = glCreateProgram();

	glAttachShader(*a_uiProgram, _result);

	glLinkProgram(*a_uiProgram);

	GLint _success = NULL;
	glGetProgramiv(*a_uiProgram, GL_LINK_STATUS, &_success);

	if (!_success)
	{
		GLint _infoLenght = NULL;
		glGetProgramiv(*a_uiProgram, GL_INFO_LOG_LENGTH, &_infoLenght);
		GLchar* _failLog = new GLchar[_infoLenght];
		glGetProgramInfoLog(*a_uiProgram, _infoLenght, NULL, _failLog);
		printf("ERROR: <SHADER PROGRAM LINKING FAILLURE> \n");
		printf("----------- REASON LINKING HAS FAILED --------- \n");
		printf("%s\n", _failLog);
		delete[] _failLog;
	}
	else
	{
		glDeleteShader(_result);
	}
}

void Dof::LoadShader()
{
	if (m_uiViewProgram)
		glDeleteProgram(m_uiViewProgram);

	m_uiViewProgram = m_oShader.GenerateShaderProgram(DOF_VERTEX_GLSL, nullptr, DOF_FRAGMENT_GLSL);

	Uniforms.View.m_iProj			= glGetUniformLocation(m_uiViewProgram, "Proj_Matrix");
	Uniforms.View.m_iMV				= glGetUniformLocation(m_uiViewProgram, "MV_Matrix");
	Uniforms.View.m_iFullShading	= glGetUniformLocation(m_uiViewProgram, "Full_Shading");
	Uniforms.View.m_iDiffuseAlb		= glGetUniformLocation(m_uiViewProgram, "Diffuse_Albedo");

	if (m_uiDisplayProgram)
		glDeleteProgram(m_uiDisplayProgram);

	m_uiDisplayProgram = m_oShader.GenerateShaderProgram(DOF_DISPLAY_VERTEX_GLSL, nullptr, DOF_DISPLAY_FRAGMENT_GLSL);

	Uniforms.DOF.m_iFocalDist  = glGetUniformLocation(m_uiDisplayProgram, "focal_distance");
	Uniforms.DOF.m_iFocalDepth = glGetUniformLocation(m_uiDisplayProgram, "focal_depth");

	if (m_uiFilterProgram)
		glDeleteProgram(m_uiFilterProgram);

	ComputeProgram(DOF_COMPUTE_GLSL, &m_uiFilterProgram);
}

void Dof::LoadMesh(const char* a_pccFileName)
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

void Dof::DrawMesh(MeshData a_oMesh)
{
	glBindVertexArray(a_oMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, a_oMesh.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}