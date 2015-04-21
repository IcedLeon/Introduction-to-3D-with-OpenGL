#include "Dof.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include "Gizzle.h"

Dof::Dof() : Application(),
			 m_oFile(nullptr),
			 m_fFocalDist(NULL),
			 m_fFocalDepth(NULL),
			 m_uiQuad_VAO(NULL),
			 m_uiViewProgram(NULL),
			 m_uiFilterProgram(NULL),
			 m_uiDisplayProgram(NULL),
			 m_bLoaded(false),
			 m_bUnlockContent(false),
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

	float _data[] =
	{
		15.0f,
		15.0f,
		15.0f,
		15.0f
	};
	int* _data2 = new int[2]
	{
		12, 12
	};

	static const char* const _objNames[] =
	{
		"./models/rigged/Pyro/pyro.fbx",
		"./models/stanford/buddha.fbx",
		"./models/stanford/bunny.fbx",
	};
	int _choosedMesh = glm::linearRand(0, 3);

	StartThreding(_objNames[1]);

	GenBuffers();
}

void Dof::LoadFBXFile(const char* a_pccFileName)
{
	printf("Loading mesh %s \n", a_pccFileName);
	m_oFile = new FBXFile();
	m_oFile->load(a_pccFileName);
	m_bUnlockContent = true;
}

void Dof::StartThreding(const char* a_pccFileName)
{
	_newThread	= std::thread(&Dof::LoadFBXFile, this, a_pccFileName);
}

void Dof::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();
	//m_oView = glm::lookAt(vec3(0.0f, 0.0f, 40.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	static const GLfloat _zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	static float _lastTime = 0.0f;
	static float _totalTime = 0.0f;

	if (!m_bPaused)
		_totalTime += (float)(a_dDeltaTime - _lastTime);

	const float _f = _totalTime * 0.12f;
	
	if (m_bUnlockContent)
	{
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
			m_oObject.push_back(LoadMesh(m_oFile));
			m_oObject[i]->m_vDiffuseAlbedo = object_colors[i];
		}
		m_bUnlockContent = !m_bUnlockContent;
		m_bLoaded		 = !m_bLoaded;
	}

	if (m_bLoaded)
	{ 
		m_oObject[0]->m_mModelTrans = glm::translate(vec3(5.0f, 0.0f, 20.0f)) *
									 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
									 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
									 glm::translate(vec3(0.0f, -4.0f, 0.0f));

		m_oObject[1]->m_mModelTrans = glm::translate(vec3(-5.0f, 0.0f, 20.0f)) *
									 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
									 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
									 glm::translate(vec3(0.0f, -4.0f, 0.0f));

		m_oObject[2]->m_mModelTrans = glm::translate(vec3(-15.0f, 0.0f, 20.0f)) *
									 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
									 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
									 glm::translate(vec3(0.0f, -4.0f, 0.0f));
		
		m_oObject[3]->m_mModelTrans = glm::translate(vec3(-25.0f, 0.0f, 20.0f)) *
									 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
									 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
									 glm::translate(vec3(0.0f, -4.0f, 0.0f));
		
		m_oObject[4]->m_mModelTrans = glm::translate(vec3(-35.0f, 0.0f, 20.0f)) *
									 glm::rotate(_f, vec3(0.0f, 1.0f, 0.0f)) *
									 glm::rotate(20.0f, vec3(1.0f, 0.0f, 0.0f)) *
									 glm::translate(vec3(0.0f, -4.0f, 0.0f));
	}
	OnKey();
	MoveCamera((float)a_dDeltaTime);
}

void Dof::CleanUpWin()
{
	_newThread.join();
	delete m_oFile;
	for (auto it : m_oObject)
	{
		delete it;
	}
	glDeleteProgram(m_uiDisplayProgram);
	glDeleteProgram(m_uiFilterProgram);
	glDeleteProgram(m_uiViewProgram);
	Application::CleanUpWin();
}

void Dof::Draw()
{
	glEnable(GL_DEPTH_TEST);

	if (m_bLoaded)
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

	unsigned int i;
	for (i = 0; i < m_oObject.size(); ++i)
	{
		mat4& _modelMat = m_oObject[i]->m_mModelTrans;
		glUniformMatrix4fv(Uniforms.View.m_iMV, 1, GL_FALSE, value_ptr(m_oView * _modelMat));
		glUniform3fv(Uniforms.View.m_iDiffuseAlb, 1, (float*)&m_oObject[i]->m_vDiffuseAlbedo);
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

MeshData* Dof::LoadMesh(FBXFile* a_oFile)
{
	MeshData* _result = new MeshData();

	unsigned int _meshCount = a_oFile->getMeshCount();

	for (unsigned int meshIndx = 0; meshIndx < _meshCount; ++meshIndx)
	{
		FBXMeshNode* _currMesh = a_oFile->getMeshByIndex(meshIndx);

		_result->m_uiIndexCount = _currMesh->m_indices.size();

		glGenBuffers(1, &_result->m_uiVBO);
		glGenBuffers(1, &_result->m_uiIBO);
		glGenVertexArrays(1, &_result->m_uiVAO);

		glBindVertexArray(_result->m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, _result->m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex) * _currMesh->m_vertices.size(),
			_currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _result->m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _currMesh->m_indices.size(),
			_currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //Position
		glEnableVertexAttribArray(1); //Normal

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
	return _result;
}

void Dof::DrawMesh(MeshData* a_oMesh)
{
	glBindVertexArray(a_oMesh->m_uiVAO);
	glDrawElements(GL_TRIANGLES, a_oMesh->m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}