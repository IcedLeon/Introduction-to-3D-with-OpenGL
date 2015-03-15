#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include "tiny_obj_loader.h"
#include <vector>

using std::vector;

struct OpenGLInfo
{
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};

struct
{
	GLint	MVP_Lht;
} Light;

struct
{
	GLint MV_Mat; //Model view matrix uniform
	GLint P_Mat; //Projection matrix uniform
	GLint Shw_Mat; //Shadow matrix uniform
	GLint Full_Shading; //Shading uniform
} Uniforms;

class ShadowMap : public Application
{
private:
	vector<OpenGLInfo>			m_oObject;
	vector<tinyobj::shape_t>	m_oShapes;
	vector<tinyobj::material_t> m_oMaterials;

public:
	ShadowMap();
	~ShadowMap();

};
#endif //!_SHADOWMAP_H_