//------------------------------------------------------------------------------
//File: ShaderCompiler.h
// Date: 25/1/15
// Brief: The Shader.h is a file specification for a shader reader wrapper,
// the main functionality of this class is to contain, and include all the 
// utility in order to compile shader and initialize the shader program.
// All the shader are going to be compiled throught .glsl, precompiled shader file.
//------------------------------------------------------------------------------------
#ifndef _SHADERCOMPILER_H_
#define _SHADERCOMPILER_H_
//Library
#include "gl_core_4_4.h"

#define SHD_ERR_LGH 512
#define VERTEX_FILE_ID	 "./shaders/VertexShader.glsl"
#define FRAGMENT_FILE_ID "./shaders/FragmentShader.glsl"

class ShaderCompiler
{
private:
	//The Shader program
	GLuint m_uiShaderProgram;
	//Compile shader, it will compile the apporopriate shader file.
	//The first one simply catch the two compatible and standard shader file available.
	GLuint CreateShader(GLenum a_eShaderID, const char* a_strShaderFileID) const;

public:
	ShaderCompiler() : m_uiShaderProgram(0)
	{ }
	~ShaderCompiler() { }
	//Init, it's self-sxplanetory. It will run the shader ruotine and compile the necessary
	//Shader compiler, it will run and compile the specific shader passed throught.
	void CreateShaderProgram(const char* a_pccVertexID, const char* a_pccFragID);
	//Get the shader program, once the shader class has been compiled all the shader are going to be linked with
	//the current program so there will be no need to having extra variable for the both compiled shaders.
	GLuint GetShaderProgram() const;
	//Use the Shader program, NOTE: DO NOT TRY TO CALL THIS FUNCTION BEFORE INIT THE SHADER CLASS.
	void Use();
	//Closing call before closing the program.
	void CleanUpProgram();
};
#endif //!_SHADERCOMPILER_H_