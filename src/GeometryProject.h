#ifndef _GEOMETRYPROJECT_H_
#define _GEOMETRYPROJECT_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

class GeometryProject : public Application
{
private:
	ShaderCompiler m_oShader;

	GLfloat m_fTimer;
	GLfloat m_fSinAug;
	GLfloat m_fTimeAug;
	GLfloat m_fModRGB;

	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;

	GLuint m_uiVAOc;
	GLuint m_uiVBOc;
	GLuint m_uiIBOc;

	GLuint m_uiIndexCount;
	GLuint m_uiCubeIndex;

	GLuint m_uiTexture;

public:
	GeometryProject();
	~GeometryProject();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot);
	void GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void GenerateCube();
	void GenerateQuad(float a_fSize);
	void IncreaseValue();
	float Hash(vec3 _p);
	void LoadTexture(const char* a_pccFileName);
};
#endif //!_GEOMETRYPROJECT_H_