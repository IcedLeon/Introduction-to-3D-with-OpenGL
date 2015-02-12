#ifndef _LIGHTINGPROJECT_H_
#define _LIGHTINGPROJECT_H_
//Lib
#include "Application.h"
#include "Vertex.h"
#include "ShaderCompiler.h"


#define VERTEX_FILE_ID1	 "./shaders/LightingVertex.glsl"
#define FRAGMENT_FILE_ID1 "./shaders/LightingFragment.glsl"

class Camera;
class FBXFile;

class Lighting : public Application
{
private:
	static Camera m_oCamera;
	ShaderCompiler m_oShader;

	mat4 m_oProjection;
	mat4 m_oView;
	mat4 m_oWorld;

	FBXFile* m_fbxFileID;

	GLfloat m_fTimer;
	GLfloat m_fSinAug;
	GLfloat m_fTimeAug;
	float _dt;
	float _modRGB;

	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;

	GLuint m_uiIndexCount;

	GLuint m_uiTexture;

	static bool	m_bKeys[1024];

public:
	Lighting();
	~Lighting();

	virtual void InitWindow(vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();

	virtual void MoveCamera(float a_fDeltaTime);

	static void key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode);
	static void scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY);
	static void mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY);
	
	void GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void Use();

	void CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void CleanupOpenGLBuffers();
	
	void SwitchShader();

	void IncreaseValue();
	void LoadTexture(const char* a_pccFileName);
};
#endif //!_LIGHTINGPROJECT_H_