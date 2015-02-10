#include "ShaderCompiler.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

GLuint ShaderCompiler::CreateShader(GLenum a_eShaderID, const GLchar* a_strShaderFileID) const
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

void ShaderCompiler::CreateShaderProgram()
{
	//Create a container for the shader
	std::vector<GLuint> _shaderPool;
	//start pushing the shader into the vector
	//First the Vertex shader
	_shaderPool.push_back(CreateShader(GL_VERTEX_SHADER, VERTEX_FILE_ID));
	//and now the frag shader
	_shaderPool.push_back(CreateShader(GL_FRAGMENT_SHADER, FRAGMENT_FILE_ID));
	//Create the shader program
	this->m_uiShaderProgram = glCreateProgram();
	//attach the shader to the program
	std::vector<GLuint>::iterator _itr = _shaderPool.begin();
	for (_itr; _itr != _shaderPool.end(); ++_itr)
	{
		glAttachShader(this->m_uiShaderProgram, *_itr);
	}
	//link the program
	glLinkProgram(this->m_uiShaderProgram);
	//Check for link errors, in the event of a falliure print the error on console.
	GLint _success;
	glGetProgramiv(this->m_uiShaderProgram, GL_LINK_STATUS, &_success);
	if (!_success)
	{
		GLchar _infoLog[SHD_ERR_LGH];
		glGetProgramInfoLog(this->m_uiShaderProgram, SHD_ERR_LGH, NULL, _infoLog);
		printf("ERROR: <SHADER PROGRAM LINKING FAILLURE> %s\n", _infoLog);
		delete[] & _infoLog;
	}
	_itr = _shaderPool.begin();
	for (_itr; _itr != _shaderPool.end(); ++_itr)
	{
		glDeleteShader(*_itr);
	}
}

GLuint ShaderCompiler::GetShaderProgram() const
{
	return m_uiShaderProgram;
}

void ShaderCompiler::EnableArrays()
{
	glBindFragDataLocation(this->m_uiShaderProgram, 0, "outColor");
	GLuint _posAttrib = glGetAttribLocation(this->m_uiShaderProgram, "position");
	glEnableVertexAttribArray(_posAttrib);
	glVertexAttribPointer(_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	GLuint _colAttrib = glGetAttribLocation(this->m_uiShaderProgram, "color");
	glEnableVertexAttribArray(_colAttrib);
	glVertexAttribPointer(_colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	//m_uiUniformCol = glGetUniformLocation(m_uiShaderProgram, "triangleColor");

	GLint _texAttrib = glGetAttribLocation(this->m_uiShaderProgram, "texcoord");
	glEnableVertexAttribArray(_texAttrib);
	glVertexAttribPointer(_texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
}

void ShaderCompiler::Use()
{
	glUseProgram(this->m_uiShaderProgram);
	EnableArrays();
}

void ShaderCompiler::CleanUpProgram()
{
	glDeleteProgram(this->m_uiShaderProgram);
}