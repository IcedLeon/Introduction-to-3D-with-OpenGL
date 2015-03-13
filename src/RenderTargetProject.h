#ifndef _RENDERTARGETP_H_
#define _RENDERTARGETP_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"

#define RENDERING_VERTEX_GLSL "./shaders/RenderingVertex.glsl"
#define RENDERING_FRAGMENT_GLSL "./shaders/RenderingFragment.glsl"

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

class RenderTargetP : public Application
{
private:
	ShaderCompiler	m_oShader;
	OpenGLInfo		m_oPlane;
	int				m_iNewSize;
	GLuint			m_uiTexture;
	GLuint			m_uiFBO;
	GLuint			m_uiFBO_T;
	GLuint			m_uiFBO_D;

public:
	RenderTargetP();
	~RenderTargetP();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_fDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void GenerateFrameBuffers();

	void GeneratePlane();
};

//struct SkyBox
//{
//	GLuint m_uiVAO;
//
//	SkyBox()
//	{
//		float _side		= 50.0f;
//		float _side2 = _side / 2;
//
//		float _vertices[24*3] = 
//		{
//			// Front
//			-_side2, -_side2, _side2,
//			 _side2, -_side2, _side2,
//			 _side2,  _side2, _side2,
//			-_side2,  _side2, _side2,
//			// Right
//			_side2, -_side2,  _side2,
//			_side2, -_side2, -_side2,
//			_side2,  _side2, -_side2,
//			_side2,  _side2,  _side2,
//			// Back
//			-_side2, -_side2, -_side2,
//			-_side2,  _side2, -_side2,
//			 _side2,  _side2, -_side2,
//			 _side2, -_side2, -_side2,
//			// Left
//			-_side2, -_side2,  _side2,
//			 _side2,  _side2,  _side2,
//			-_side2,  _side2, -_side2,
//			-_side2, -_side2, -_side2,
//			// Bottom
//			-_side2, -_side2,  _side2,
//			-_side2, -_side2, -_side2,
//			 _side2, -_side2, -_side2,
//			 _side2, -_side2,  _side2,
//			// Top
//			-_side2, _side2,  _side2,
//			 _side2, _side2,  _side2,
//			 _side2, _side2, -_side2,
//			-_side2, _side2, -_side2
//		};
//
//		GLuint _index[] = 
//		{
//			0, 2, 1, 0, 3, 2,
//			4, 6, 5, 4, 7, 6,
//			8, 10, 9, 8, 11, 10,
//			12, 14, 13, 12, 15, 14,
//			16, 18, 17, 16, 19, 18,
//			20, 22, 21, 20, 23, 22
//		};
//
//		GLuint _handle[2];
//		glGenBuffers(2, _handle);
//
//		glBindBuffer(GL_ARRAY_BUFFER, _handle[0]);
//		glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(float), _vertices, GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle[1]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), _index, GL_STATIC_DRAW);
//
//		glGenVertexArrays(1, &m_uiVAO);
//		glBindVertexArray(m_uiVAO);
//
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
//		glEnableVertexAttribArray(0);  // Vertex position
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle[1]);
//
//		glBindVertexArray(0);
//	}
//
//	void Render() const
//	{
//		glBindVertexArray(m_uiVAO);
//		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
//	}
//};
//
//struct VBOPlane
//{
//	int		m_iFaces;
//	GLuint	m_uiVAO;
//
//	VBOPlane(float	a_fXSize,
//			 float	a_fZSize,
//			 int	a_iXDivs,
//			 int	a_iZDivs,
//			 float	a_fSMax,
//			 float	a_fTMax)
//	{
//
//		m_iFaces = a_iXDivs * a_iZDivs;
//		float * v = new float[3 * (a_iXDivs + 1) * (a_iZDivs + 1)];
//		float * n = new float[3 * (a_iXDivs + 1) * (a_iZDivs + 1)];
//		float * tex = new float[2 * (a_iXDivs + 1) * (a_iZDivs + 1)];
//		unsigned int * el = new unsigned int[6 * a_iXDivs * a_iZDivs];
//
//		float x2 = a_fXSize / 2.0f;
//		float z2 = a_fZSize / 2.0f;
//		float iFactor = (float)a_fZSize / a_iZDivs;
//		float jFactor = (float)a_fXSize / a_iXDivs;
//		float texi = a_fSMax / a_iZDivs;
//		float texj = a_fTMax / a_iXDivs;
//		float x, z;
//		int vidx = 0, tidx = 0;
//		for (int i = 0; i <= a_iZDivs; i++) {
//			z = iFactor * i - z2;
//			for (int j = 0; j <= a_iZDivs; j++) {
//				x = jFactor * j - x2;
//				v[vidx] = x;
//				v[vidx + 1] = 0.0f;
//				v[vidx + 2] = z;
//				n[vidx] = 0.0f;
//				n[vidx + 1] = 1.0f;
//				n[vidx + 2] = 0.0f;
//				vidx += 3;
//				tex[tidx] = j * texi;
//				tex[tidx + 1] = i * texj;
//				tidx += 2;
//			}
//		}
//
//		unsigned int rowStart, nextRowStart;
//		int idx = 0;
//		for (int i = 0; i < zdivs; i++) {
//			rowStart = i * (xdivs + 1);
//			nextRowStart = (i + 1) * (xdivs + 1);
//			for (int j = 0; j < xdivs; j++) {
//				el[idx] = rowStart + j;
//				el[idx + 1] = nextRowStart + j;
//				el[idx + 2] = nextRowStart + j + 1;
//				el[idx + 3] = rowStart + j;
//				el[idx + 4] = nextRowStart + j + 1;
//				el[idx + 5] = rowStart + j + 1;
//				idx += 6;
//			}
//		}
//
//		unsigned int handle[4];
//		glGenBuffers(4, handle);
//
//		glGenVertexArrays(1, &vaoHandle);
//		glBindVertexArray(vaoHandle);
//
//		glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
//		glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), v, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
//		glEnableVertexAttribArray(0);  // Vertex position
//
//		glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
//		glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), n, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
//		glEnableVertexAttribArray(1);  // Vertex normal
//
//		glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
//		glBufferData(GL_ARRAY_BUFFER, 2 * (xdivs + 1) * (zdivs + 1) * sizeof(float), tex, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
//		glEnableVertexAttribArray(2);  // Texture coords
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * xdivs * zdivs * sizeof(unsigned int), el, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//
//		delete[] v;
//		delete[] n;
//		delete[] tex;
//		delete[] el;
//	}
//};
//
//class RenderTargetP2 : public Application
//{
//private:
//	ShaderCompiler	m_oShader;
//	OpenGLInfo		m_oPlane;
//	SkyBox*			m_oSky;
//	int				m_iNewSize;
//	GLuint			m_uiTexture;
//	GLuint			m_uiFBO;
//	GLuint			m_uiFBO_T;
//	GLuint			m_uiFBO_D;
//
//public:
//	RenderTargetP2();
//	~RenderTargetP2();
//
//	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
//	virtual void Update(GLdouble a_fDeltaTime);
//	virtual void CleanUpWin();
//	virtual void Draw();
//	virtual void MoveCamera(float a_fDeltaTime);
//	virtual void Use();
//
//	void SetMatrices();
//	void LoadCubeMap(const char* a_pccFileName);
//	void GenerateFrameBuffers();
//
//	void GeneratePlane();
//};
#endif //!_RENDERTARGETP_H_