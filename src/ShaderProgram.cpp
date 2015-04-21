#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

namespace SHADERINFO
{
	struct Shader_File_ext
	{
		const char*				_pccExt;
		GLSLShader::ShaderType	_eType;
	};
	
	Shader_File_ext Extensions[] = 
	{
		{ ".vs",	GLSLShader::VERTEX },
		{ ".vert",	GLSLShader::VERTEX },
		{ ".gs",	GLSLShader::GEOMETRY },
		{ ".geom",	GLSLShader::GEOMETRY },
		{ ".tcs",	GLSLShader::TESS_CONTROL },
		{ ".tes",	GLSLShader::TESS_EVALUATION },
		{ ".fs",	GLSLShader::FRAGMENT },
		{ ".frag",	GLSLShader::FRAGMENT },
		{ ".cs",	GLSLShader::COMPUTE }
	};
}

ShaderProgram::ShaderProgram() : m_uiPrgHandle(NULL),
								 m_bLinked(false)
{ }

ShaderProgram::ShaderProgram(const ShaderProgram& a_Rhs)
{
	this->m_uiPrgHandle			= a_Rhs.m_uiPrgHandle;
	this->m_bLinked				= a_Rhs.m_bLinked;
	this->m_oUniformLocations	= a_Rhs.m_oUniformLocations;
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_uiPrgHandle);
}

void ShaderProgram::CompileShader(const char* a_strShaderFileID)
{
	//I determine the number of Extensions available.
	int _numOfExt = sizeof(SHADERINFO::Extensions) / sizeof(SHADERINFO::Shader_File_ext);
	//The current Extensions.
	string _currExt = GetExtension(a_strShaderFileID);
	//It's just for filling and avoiding junk to be stored, 
	//if the compilation fail to find the match it will return an error.
	GLSLShader::ShaderType _type = GLSLShader::VERTEX;
	//A flag to be raised upon match is found
	bool _hasMatch = false;

	for (int i = 0; i < _numOfExt; ++i)
	{
		if (_currExt == SHADERINFO::Extensions[i]._pccExt)
		{
			_hasMatch = true;
			_type = SHADERINFO::Extensions[i]._eType;
			break;
		}
	}

	if (!_hasMatch)
	{
		const char* _errMsg = "<ERROR>: Extension could not be recognized: %s";
		printf(_errMsg, _currExt);
	}
	CompileShader(a_strShaderFileID, _type);
}

void ShaderProgram::CompileShader(const char* a_strShaderFileID, GLenum a_eShaderID)
{
	const char* _errMsg;

	if (!IsFileIDExists(*a_strShaderFileID))
	{
		_errMsg = "<ERROR> Shader: %s could not be found.\n";
		printf(_errMsg, a_strShaderFileID);
	}

	if (this->m_uiPrgHandle <= 0)
	{
		m_uiPrgHandle = glCreateProgram();
		if (this->m_uiPrgHandle == 0)
		{
			_errMsg = "<ERROR>: Unable to create shader program.\n";
			printf(_errMsg);
		}
	}

	std::ifstream _shaderFile(a_strShaderFileID, std::ios::in);

	if (!_shaderFile)
	{
		_errMsg = "<ERROR>: Unable to open and read: %s";
		printf(_errMsg, a_strShaderFileID);
	}

	std::stringstream _shaderCode;

	_shaderCode << _shaderFile.rdbuf();
	_shaderFile.close();

	string _stringToConvert = _shaderCode.str();

	const char* _compiledShrCode = _stringToConvert.c_str();

	GLint _success;
	GLuint _shaderHandle = glCreateShader(a_eShaderID);

	glShaderSource(_shaderHandle, 1, &_compiledShrCode, NULL);
	glCompileShader(_shaderHandle);

	glGetShaderiv(_shaderHandle, GL_COMPILE_STATUS, &_success);

	if (!_success)
	{
		int LOG_LHG = NULL;
		glGetShaderiv(_shaderHandle, GL_INFO_LOG_LENGTH, &LOG_LHG);

		if (LOG_LHG > 0)
		{
			GLchar* _infoLog = new GLchar[LOG_LHG];
			GLint _written = NULL;
			glGetShaderInfoLog(_shaderHandle, LOG_LHG, &_written, _infoLog);
			_errMsg = "<ERROR>: Shader compilation has failed. File: %s.\n %s";
			printf(_errMsg, a_strShaderFileID, _infoLog);
			delete[] _infoLog;
		}
	}
	else
	{
		glAttachShader(this->m_uiPrgHandle, _shaderHandle);
	}
}

void ShaderProgram::LinkProgram()
{
	if (m_bLinked)
		return;

	glLinkProgram(this->m_uiPrgHandle);

	GLint _success = NULL;
	glGetProgramiv(this->m_uiPrgHandle, GL_LINK_STATUS, &_success);
	if (!_success)
	{
		int LOG_LHG = NULL;
		glGetProgramiv(this->m_uiPrgHandle, GL_INFO_LOG_LENGTH, &LOG_LHG);
		if (LOG_LHG > 0)
		{
			GLchar* _infoLog = new GLchar[LOG_LHG];
			int _written = NULL;
			glGetProgramInfoLog(this->m_uiPrgHandle, LOG_LHG, &_written, _infoLog);
			printf("<ERROR>: Shader program linking has failed: %s\n", _infoLog);
			delete[] _infoLog;
		}
	}
	else
	{
		m_bLinked = true;
	}
}

string ShaderProgram::GetExtension(const char* a_pccFileName)
{
	string _toString(a_pccFileName);

	size_t _loc = _toString.find_last_of('.');

	if (_loc != string::npos)
	{
		return _toString.substr(_loc, string::npos);
	}
	
	return "";
}

GLint ShaderProgram::GetUniformLocation(const char* a_pccUniformName)
{
	map<string, GLint>::iterator hash;
	hash = m_oUniformLocations.find(a_pccUniformName);

	if (hash == m_oUniformLocations.end())
	{
		m_oUniformLocations[a_pccUniformName] = glGetUniformLocation(this->m_uiPrgHandle, a_pccUniformName);
	}

	return m_oUniformLocations[a_pccUniformName];
}

bool ShaderProgram::IsFileIDExists(const char& a_sFileName) const
{
	struct stat info;

	int _ret = -1;

	_ret = stat(&a_sFileName, &info);

	return 0 == _ret;
}

void ShaderProgram::CreateProgram(vector<const char*> a_pccShaderIDs)
{
	std::vector<const char*>::iterator itr;
	for (itr = a_pccShaderIDs.begin(); itr != a_pccShaderIDs.end(); ++itr)
	{
		CompileShader(*itr);
	}

	LinkProgram();

	if (m_bLinked)
	{
		GLint _shaderNum = NULL;
		glGetProgramiv(this->m_uiPrgHandle, GL_ATTACHED_SHADERS, &_shaderNum);
		GLuint* _shaderName = new GLuint[_shaderNum];
		glGetAttachedShaders(this->m_uiPrgHandle, _shaderNum, NULL, _shaderName);
		for (int i = 0; i < _shaderNum; ++i)
		{
			glDeleteShader(_shaderName[i]);
		}
	}
	m_oUniformLocations.clear();
}

void ShaderProgram::CreateProgramPipeline(const char* a_pccShrSource)
{
	//I determine the number of Extensions available.
	int _numOfExt = sizeof(SHADERINFO::Extensions) / sizeof(SHADERINFO::Shader_File_ext);
	//The current Extensions.
	string _currExt = GetExtension(a_pccShrSource);
	//It's just for filling and avoiding junk to be stored, 
	//if the compilation fail to find the match it will return an error.
	GLSLShader::ShaderType _type = GLSLShader::VERTEX;
	//A flag to be raised upon match is found
	bool _hasMatch = false;

	for (int i = 0; i < _numOfExt; ++i)
	{
		if (_currExt == SHADERINFO::Extensions[i]._pccExt)
		{
			_hasMatch = true;
			_type = SHADERINFO::Extensions[i]._eType;
			break;
		}
	}

	if (!_hasMatch)
	{
		const char* _errMsg = "<ERROR>: Extension could not be recognized: %s";
		printf(_errMsg, _currExt);
	}
	else
	{
		const char* _errMsg;

		if (!IsFileIDExists(*a_pccShrSource))
		{
			_errMsg = "<ERROR> Shader: %s could not be found.\n";
			printf(_errMsg, a_pccShrSource);
		}

		//if (this->m_uiPrgHandle <= 0)
		//{
		//	m_uiPrgHandle = glCreateProgram();
		//	if (this->m_uiPrgHandle == 0)
		//	{
		//		_errMsg = "<ERROR>: Unable to create shader program.\n";
		//		printf(_errMsg);
		//	}
		//}

		std::ifstream _shaderFile(a_pccShrSource, std::ios::in);

		if (!_shaderFile)
		{
			_errMsg = "<ERROR>: Unable to open and read: %s";
			printf(_errMsg, a_pccShrSource);
		}

		std::stringstream _shaderCode;

		_shaderCode << _shaderFile.rdbuf();
		_shaderFile.close();

		string _stringToConvert = _shaderCode.str();

		const char* _compiledShrCode = _stringToConvert.c_str();

		this->m_uiPrgHandle = glCreateShaderProgramv(_type, 2, &_compiledShrCode);

		GLint _success = NULL;
		glGetProgramiv(this->m_uiPrgHandle, GL_LINK_STATUS, &_success);
		if (!_success)
		{
			int LOG_LHG = NULL;
			glGetProgramiv(this->m_uiPrgHandle, GL_INFO_LOG_LENGTH, &LOG_LHG);
			if (LOG_LHG > 0)
			{
				GLchar* _infoLog = new GLchar[LOG_LHG];
				int _written = NULL;
				glGetShaderInfoLog(this->m_uiPrgHandle, LOG_LHG, &_written, _infoLog);
				printf("<ERROR>: Shader program linking has failed:\n%s\n", _infoLog);
				delete[] _infoLog;
			}
		}
		else
		{
			m_bLinked = true;
		}
	}
}

void ShaderProgram::ValidateProgram()
{
	if (!m_bLinked)
	{
		printf("<WARNING>: Program could not be validate because has not being linked priorly.\n");
	}

	GLint _status = NULL;
	
	glValidateProgram(this->m_uiPrgHandle);

	glGetProgramiv(this->m_uiPrgHandle, GL_VALIDATE_STATUS, &_status);

	if (!_status)
	{
		int LOG_LHG = NULL;

		glGetProgramiv(this->m_uiPrgHandle, GL_INFO_LOG_LENGTH, &LOG_LHG);

		if (LOG_LHG > 0)
		{
			GLchar* _infoLog = new GLchar[LOG_LHG];
			GLint _written = NULL;

			glGetProgramInfoLog(this->m_uiPrgHandle, LOG_LHG, &_written, _infoLog);
			printf("<ERROR>: Program failed validation \n", _infoLog);
			delete[] _infoLog;
		}
	}
}

void ShaderProgram::Use()
{
	if (this->m_uiPrgHandle <= 0 || (!m_bLinked))
		printf("<WARNING>: Shader program has not being linked, be sure to initialize it before use it.");
	glUseProgram(this->m_uiPrgHandle);
}

void ShaderProgram::Disable()
{
	glUseProgram(NULL);
}

void ShaderProgram::Release()
{
	glDeleteProgram(this->m_uiPrgHandle);
}

GLuint ShaderProgram::GetHandle() const
{
	return this->m_uiPrgHandle;
}

bool ShaderProgram::IsLinked() const
{
	return m_bLinked;
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const vec2& a_vVector)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform2fv(_loc, 1, (float*)&a_vVector);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const vec3& a_vVector)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform3fv(_loc, 1, (float*)&a_vVector);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const vec4& a_vVector)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform4fv(_loc, 1, (float*)&a_vVector);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const vec4& a_vVector, int a_iNumOfVec)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform4fv(_loc, a_iNumOfVec, (float*)&a_vVector);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const mat3& a_mMatrix)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniformMatrix3fv(_loc, 1, GL_FALSE, value_ptr(a_mMatrix));
}

void ShaderProgram::SetUniform(const char* a_pccUniName, const mat4& a_mMatrix)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniformMatrix4fv(_loc, 1, GL_FALSE, value_ptr(a_mMatrix));
}

void ShaderProgram::SetUniform(const char* a_pccUniName, float a_fX, float a_fY, float a_fZ, float a_fW)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform4f(_loc, a_fX, a_fY, a_fZ, a_fW);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, float a_fX, float a_fY, float a_fZ)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform3f(_loc, a_fX, a_fY, a_fZ);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, float a_fX, float a_fY)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform2f(_loc, a_fX, a_fY);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, float a_fValue)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform1f(_loc, a_fValue);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, int a_iValue)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform1i(_loc, a_iValue);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, GLuint a_uiValue)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform1ui(_loc, a_uiValue);
}

void ShaderProgram::SetUniform(const char* a_pccUniName, bool a_bFlag)
{
	GLint _loc = GetUniformLocation(a_pccUniName);
	glUniform1i(_loc, a_bFlag);
}

void ShaderProgram::ListActiveUniform()
{
	GLint _numOfUni = NULL;
	glGetProgramInterfaceiv(this->m_uiPrgHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &_numOfUni);

	GLenum _properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	printf("List of active uniform:\n");
	for (int i = 0; i < _numOfUni; ++i)
	{
		GLint _result[4];
		glGetProgramResourceiv(this->m_uiPrgHandle, GL_UNIFORM, i, 4, _properties, 4, NULL, _result);

		if (_result[3] != -1)
			continue; //This serve to skip uniforms in blocks.
		GLint _nameBuffSize = _result[0] + 1;
		char* _name = new char[_nameBuffSize];
		glGetProgramResourceName(this->m_uiPrgHandle, GL_UNIFORM, i, _nameBuffSize, NULL, _name);
		printf("[DATA]:%-5d [NAME]:%s [TYPE]:%s.\n", _result[2], _name, GetTypeString(_result[1]));
		delete[] _name;
	}
}

void ShaderProgram::ListActiveUniformBlocks()
{
	GLint _numOfUniBlocks = 0;

	glGetProgramInterfaceiv(this->m_uiPrgHandle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &_numOfUniBlocks);
	GLenum _blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
	GLenum _blockIndex[] = { GL_ACTIVE_VARIABLES };
	GLenum _properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

	for (int block = 0; block < _numOfUniBlocks; ++block)
	{
		GLint _blockInfo[2];
		glGetProgramResourceiv(this->m_uiPrgHandle, GL_UNIFORM_BLOCK, block, 2, _blockProps, 2, NULL, _blockInfo);
		GLint _numUnis = _blockInfo[0];

		GLchar* _blockName = new GLchar[_blockInfo[1] + 1];
		glGetProgramResourceName(this->m_uiPrgHandle, GL_UNIFORM_BLOCK, block, _blockInfo[1] + 1, NULL, _blockName);
		printf("Uniform block \"%s\":\n", _blockName);
		delete[] _blockName;

		GLint* _unifIndexes = new GLint[_numUnis];
		glGetProgramResourceiv(this->m_uiPrgHandle, GL_UNIFORM_BLOCK, block, 1, _blockIndex, _numUnis, NULL, _unifIndexes);

		for (int unif = 0; unif < _numUnis; ++unif)
		{
			GLint _uniIndex = _unifIndexes[unif];
			GLint _results[3];
			glGetProgramResourceiv(this->m_uiPrgHandle, GL_UNIFORM, _uniIndex, 3, _properties, 3, NULL, _results);

			GLint _nameBufSize = _results[0] + 1;
			GLchar* _name = new GLchar[_nameBufSize];
			glGetProgramResourceName(this->m_uiPrgHandle, GL_UNIFORM, _uniIndex, _nameBufSize, NULL, _name);
			printf("    %s (%s)\n", _name, GetTypeString(_results[1]));
			delete[] _name;
		}

		delete[] _unifIndexes;
	}
}

const char* ShaderProgram::GetTypeString(GLenum a_eType)
{
	switch (a_eType)
	{
	case GL_FLOAT:
		return "float";
		break;
	case GL_FLOAT_VEC2:
		return "vec2";
		break;
	case GL_FLOAT_VEC3:
		return "vec3";
		break;
	case GL_FLOAT_VEC4:
		return "vec4";
		break;
	case GL_DOUBLE:
		return "double";
		break;
	case GL_INT:
		return "int";
		break;
	case GL_UNSIGNED_INT:
		return "unsigned int";
		break;
	case GL_BOOL:
		return "bool";
		break;
	case GL_FLOAT_MAT2:
		return "mat2";
		break;
	case GL_FLOAT_MAT3:
		return "mat3";
		break;
	case GL_FLOAT_MAT4:
		return "mat4";
		break;
	default:
		return "Undefined";
		break;
	}
}