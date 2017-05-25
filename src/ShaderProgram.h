//File: ShaderProgram.h
// Date: 25/1/15
// Brief: The file specification for ShaderProgram.h which is a shader wrapper.
// The main functionality of this class is to contain, and include all the 
// utility in order to compile shader and initialize the shader program.
// All the shader are going to be compiled throught .glsl, precompiled shader file.
//------------------------------------------------------------------------------------
#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_
//Lib
#include "BaseInclude.h"
#include <vector>
//Using macro
using std::vector;

namespace GLSLShader
{
	enum ShaderType
	{
		VERTEX			= GL_VERTEX_SHADER,
		FRAGMENT		= GL_FRAGMENT_SHADER,
		GEOMETRY		= GL_GEOMETRY_SHADER,
		TESS_CONTROL	= GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE			= GL_COMPUTE_SHADER
	};
}

class ShaderProgram
{
private:
	GLuint m_uiPrgHandle; //The current program handle.
	bool m_bLinked;	//Track if the program has been linked before being used.
	//It will store all the current uniform location for the associated program.
	map<string, GLint> m_oUniformLocations; 
	//The copy ctr and the assign operator overload are marked as private to make the object non-copyable
	//from the extern.
	ShaderProgram(const ShaderProgram& a_Rhs);
	ShaderProgram& operator= (const ShaderProgram& a_Rhs) { return *this; }
	//Well, not much to say about this one. Has to recieve the correct shader type id and the file
	//and it will compile the shader to be used.
	void CompileShader(const char* a_strShaderFileID);
	void CompileShader(const char* a_strShaderFileID, GLenum a_eShaderID);
	//
	void LinkProgram();
	//This function will parse the file name and get the correct extension to compile the shader.
	//This should avoid tedious and passing a long ass sequence of variable into CreateProgram.
	string GetExtension(const char* a_pccFileName);
	//Internal use only, simply wrap the glGetUniformLocation func.
	GLint GetUniformLocation(const char* a_pccUniformName);
	//This function will check before the compilation of the shader if the current file do or do not exist.
	//90% of the time, if a file has not being locatet is because vs has resetted the data folder lookup or
	//simply a typo. But it's always nice to have check along this shader pipeline.
	bool IsFileIDExists(const char& a_sFileName) const;

public:
	ShaderProgram();
	~ShaderProgram();
	
	void CreateProgram(vector<const char*> a_pccShaderIDs);
	void CreateProgramPipeline(const char* a_pccShrSource);
	void ValidateProgram();
	void Use();
	void Disable();
	void Release();
	//Get the shader program, once the shader class has been compiled all the shader are going to be linked with
	//the current program so there will be no need for having extra variable for the both compiled shaders.
	GLuint GetHandle() const;
	bool IsLinked() const;

	
	void SetUniform(const char* a_pccUniName, const vec2& a_vVector);
	void SetUniform(const char* a_pccUniName, const vec3& a_vVector);
	void SetUniform(const char* a_pccUniName, const vec4& a_vVector);
	void SetUniform(const char* a_pccUniName, const vec4& a_vVector, int a_iNumOfVec);
	void SetUniform(const char* a_pccUniName, const mat3& a_mMatrix);
	void SetUniform(const char* a_pccUniName, const mat4& a_mMatrix);
	void SetUniform(const char* a_pccUniName, const mat4& a_mMatrix, int a_iNumOfMat);
	void SetUniform(const char* a_pccUniName, float a_fX, float a_fY, float a_fZ, float a_fW);
	void SetUniform(const char* a_pccUniName, float a_fX, float a_fY, float a_fZ);
	void SetUniform(const char* a_pccUniName, float a_fX, float a_fY);
	void SetUniform(const char* a_pccUniName, float a_fValue);
	void SetUniform(const char* a_pccUniName, int a_iValue);
	void SetUniform(const char* a_pccUniName, GLuint a_uiValue);
	void SetUniform(const char* a_pccUniName, bool a_bFlag);

	void ListActiveUniform();
	void ListActiveUniformBlocks();

	const char* GetTypeString(GLenum a_eType);
};
#endif //!_SHADERPROGRAM_H_
