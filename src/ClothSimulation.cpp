#include "ClothSimulation.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "TextureLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define PRIM_RESTART 0xffffff

GLuint CustomComputeShaderLoader(GLenum a_eShaderID, const GLchar* a_strShaderFileID)
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

void CustomComputeProgram(const char* a_pccComputeID, GLuint* a_uiProgram)
{
	GLuint _result = NULL;

	if (a_pccComputeID == nullptr)
	{
		printf("ERROR: <YOUR COMPUTE SHADER: %s, COULD NOT BE FOUND. CHECK PATH OR VS PROPERTIES>", a_pccComputeID);
	}
	else
	{
		_result = CustomComputeShaderLoader(GL_COMPUTE_SHADER, a_pccComputeID);
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

ClothSimulation::ClothSimulation() : m_vClothSize(vec2(NULL)),
									 m_viParticleNum(ivec2(NULL)),
									 m_fSpeed(NULL),
									 m_uiRenderProgram(NULL),
									 m_uiComputeProgram(NULL),
									 m_uiComputeNormProgram(NULL),
									 m_uiReadBuff(NULL),
									 m_uiNormBuff(NULL),
									 m_uiElBuff(NULL),
									 m_uiTCBuff(NULL)
{
	m_uiPosBuff[2]			= { NULL };
	m_uiVelBuff[2]			= { NULL };
	ClothData.m_uiVAO		= NULL;
	ClothData.m_uiIndices	= NULL;
}

ClothSimulation::~ClothSimulation()
{

}

void ClothSimulation::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIM_RESTART);

	m_oProjection = m_oCamera.GetProjectionTransform(a_vScreenSize.xy(), 1.0f, 100.0f);

	LoadShaders();

	m_vClothSize	= vec2(4.0f, 3.0f);
	m_viParticleNum = ivec2(40, 40);
	m_fSpeed		= 8.0f;

	CreateBuffer();

	SetUniform();
}

void ClothSimulation::Update(GLdouble a_dDeltaTime)
{
	m_oView		= glm::lookAt(vec3(2, 2, 5), vec3(2, 1, 0), vec3(0, 1, 0));
	m_oWorld	= mat4(1.0f);

	MoveCamera((float)a_dDeltaTime);
}

void ClothSimulation::CleanUpWin()
{
	glDeleteProgram(m_uiComputeNormProgram);
	glDeleteProgram(m_uiComputeProgram);
	glDeleteProgram(m_uiRenderProgram);
	Application::CleanUpWin();
}

void ClothSimulation::Draw()
{
	glUseProgram(m_uiComputeProgram);
	for (unsigned int i = 0; i < 1000; ++i)
	{
		glDispatchCompute(m_viParticleNum.x / 10, m_viParticleNum.y / 10, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		//Swap buffer
		m_uiReadBuff = 1 - m_uiReadBuff;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_uiPosBuff[m_uiReadBuff]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_uiPosBuff[1-m_uiReadBuff]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_uiVelBuff[m_uiReadBuff]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_uiVelBuff[1-m_uiReadBuff]);
	}

	//Compute the normals
	glUseProgram(m_uiComputeNormProgram);
	glDispatchCompute(m_viParticleNum.x / 10, m_viParticleNum.y / 10, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	//Render pass
	//glUseProgram(m_uiRenderProgram); //Redoundant?! HERE!!!! CHECK IF IT WORK SWITCHED OFF.

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Use();
	//Last render pass, draw the cloth.
	glBindVertexArray(ClothData.m_uiVAO);
	glDrawElements(GL_TRIANGLE_STRIP, ClothData.m_uiIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(NULL);
}

void ClothSimulation::MoveCamera(float a_fDeltaTime)
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

void ClothSimulation::Use()
{
	glUseProgram(m_uiRenderProgram);
	mat4 _MV			= m_oView * m_oWorld;
	mat4 _MVP			= m_oProjection * _MV;
	glm::mat3 _normMat	= glm::mat3(_MV[0].xyz(), _MV[1].xyz(), _MV[2].xyz() );

	GLint _mvUnif	= glGetUniformLocation(m_uiRenderProgram, "ModelViewMatrix");
	GLint _normUnif = glGetUniformLocation(m_uiRenderProgram, "NormalMatrix");
	GLint _mvpUnif	= glGetUniformLocation(m_uiRenderProgram, "MVP");

	glUniformMatrix4fv(_mvUnif, 1, GL_FALSE, value_ptr(_MV));
	glUniformMatrix4fv(_normUnif, 1, GL_FALSE, value_ptr(_normMat));
	glUniformMatrix4fv(_mvpUnif, 1, GL_FALSE, value_ptr(_MVP));
}

void ClothSimulation::CreateBuffer()
{
	//Starting position for the cloth
	mat4 _clothTransform = glm::translate(mat4(1.0f), vec3(0, m_vClothSize.y, 0));
		 _clothTransform = glm::rotate(_clothTransform, glm::radians(-80.0f), vec3(1, 0, 0));
		 _clothTransform = glm::translate(_clothTransform, vec3(0, -m_vClothSize.y, 0));

	std::vector<GLfloat> _initPos;
	std::vector<GLfloat> _initVel(m_viParticleNum.x * m_viParticleNum.y * 2, 0.0f);
	std::vector<float>   _initTex;

	float _dX = m_vClothSize.x / (m_viParticleNum.x - 1);
	float _dY = m_vClothSize.y / (m_viParticleNum.y - 1);
	float _dS = 1.0f / (m_viParticleNum.x - 1);
	float _dT = 1.0f / (m_viParticleNum.y - 1);

	vec4 _point(0.0f, 0.0f, 0.0f, 1.0f);

	int i,
		j;
	for (i = 0; i < m_viParticleNum.y; ++i)
	{
		for (j = 0; j < m_viParticleNum.x; ++j)
		{
			_point.x = _dX * j;
			_point.y = _dY * i;
			_point.z = 0.0f;
			_point = _clothTransform * _point;

			_initPos.push_back(_point.x);
			_initPos.push_back(_point.y);
			_initPos.push_back(_point.z);
			_initPos.push_back(1.0f);

			_initTex.push_back(_dS * j);
			_initTex.push_back(_dT * i);
		}
	}

	std::vector<GLuint> _elm;
	int rw,
		cl;
	for (rw = 0; rw < m_viParticleNum.y - 1; ++rw)
	{
		for(cl = 0; cl < m_viParticleNum.x - 1; ++cl)
		{
			_elm.push_back((rw + 1) * m_viParticleNum.x + (cl));
			_elm.push_back((rw)* m_viParticleNum.x + (cl));
		}
		_elm.push_back(PRIM_RESTART);
	}

	GLuint _buff[7];
	glGenBuffers(7, _buff);
	m_uiPosBuff[0]	= _buff[0];
	m_uiPosBuff[1]	= _buff[1];
	m_uiVelBuff[0]	= _buff[2];
	m_uiVelBuff[1]	= _buff[3];
	m_uiNormBuff	= _buff[4];
	m_uiElBuff		= _buff[5];
	m_uiTCBuff		= _buff[6];

	GLuint _parts = m_viParticleNum.x * m_viParticleNum.y;
	//Postion buffering
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_uiPosBuff[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _parts * 4 * sizeof(GLfloat), &_initPos[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_uiPosBuff[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//velocity buffering
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_uiVelBuff[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _parts * 4 * sizeof(GLfloat), &_initPos[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_uiVelBuff[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);
	//Normal buffering
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_uiNormBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _parts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);
	//Element indicies
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_uiElBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _elm.size() * sizeof(GLuint), &_elm[0], GL_DYNAMIC_COPY);
	//texture buffering
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_uiTCBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _initTex.size() * sizeof(GLuint), &_initTex[0], GL_DYNAMIC_DRAW);
	//Recond the indicies
	ClothData.m_uiIndices = _elm.size();
	//Set the handle
	glGenVertexArrays(1, &ClothData.m_uiVAO);
	glBindVertexArray(ClothData.m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiPosBuff[0]);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiNormBuff);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiTCBuff);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiElBuff);
	glBindVertexArray(NULL);
}

void ClothSimulation::LoadShaders()
{
	m_uiRenderProgram = m_oShader.GenerateShaderProgram(ADS_VERTEX, nullptr, ADS_FRAGMENT);

	CustomComputeProgram(CLOTH_COMPUTE, &m_uiComputeProgram);
	CustomComputeProgram(CLOTH_NORMAL_COMPUTE, &m_uiComputeNormProgram);
}

void ClothSimulation::SetUniform()
{
	glUseProgram(m_uiRenderProgram);

	RenderUni.m_iLightPos				= glGetUniformLocation(m_uiRenderProgram, "LightPosition");
	RenderUni.m_iLightIntensity			= glGetUniformLocation(m_uiRenderProgram, "LightIntensity");
	RenderUni.m_iDiffuseReflectivity	= glGetUniformLocation(m_uiRenderProgram, "Kd");
	RenderUni.m_iAmbientReflectivity	= glGetUniformLocation(m_uiRenderProgram, "Ka");
	RenderUni.m_iSpecularReflectivity	= glGetUniformLocation(m_uiRenderProgram, "Ks");
	RenderUni.m_iSpecularShininessFac	= glGetUniformLocation(m_uiRenderProgram, "Shininess");

	glUniform4fv(RenderUni.m_iLightPos, 1, (float*)&vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glUniform3fv(RenderUni.m_iLightIntensity, 1, (float*)&vec3(1.0f));
	glUniform3fv(RenderUni.m_iDiffuseReflectivity, 1, (float*)&vec3(0.8f));
	glUniform3fv(RenderUni.m_iAmbientReflectivity, 1, (float*)&vec3(0.2f));
	glUniform3fv(RenderUni.m_iSpecularReflectivity, 1, (float*)&vec3(0.2f));
	glUniform1f(RenderUni.m_iSpecularShininessFac, 80.0f);

	glUseProgram(m_uiComputeProgram);

	ComputeUni.m_iRestLenghtHoriz	= glGetUniformLocation(m_uiComputeProgram, "RestLengthHoriz");
	ComputeUni.m_iRestLenghtVert	= glGetUniformLocation(m_uiComputeProgram, "RestLengthVert");
	ComputeUni.m_iRestLenghtDiag	= glGetUniformLocation(m_uiComputeProgram, "RestLengthDiag");

	float _dX = m_vClothSize.x / (m_viParticleNum.x - 1);
	float _dY = m_vClothSize.y / (m_viParticleNum.y - 1);

	glUniform1f(ComputeUni.m_iRestLenghtHoriz, _dX);
	glUniform1f(ComputeUni.m_iRestLenghtVert, _dY);
	float _restLDiag = sqrtf(_dX * _dX + _dY * _dY);
	glUniform1f(ComputeUni.m_iRestLenghtDiag, _restLDiag);

	glActiveTexture(GL_TEXTURE0);
	TEXLOADER::LoadTexture("./textures/me_textile.tga");
}