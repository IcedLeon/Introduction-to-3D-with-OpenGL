#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include "tiny_obj_loader.h"
#include <vector>
//Using macro
using std::vector;
//Macro
#define DEPTH_TEXTURE_SIZE      4096
#define FRUSTUM_DEPTH           1000
//Lighting shader
#define SHADOW_LIGHT_VERTEX		"./shaders/Shadow_light_vertex.glsl"
#define SHADOW_LIGHT_FRAGMENT	"./shaders/Shadow_light_fragment.glsl"
//Camera shader
#define SHADOW_CAMERA_VERTEX	"./shaders/Shadow_camera_vertex.glsl"
#define SHADOW_CAMERA_FRAGMENT	"./shaders/Shadow_camera_fragment.glsl"
//depth view shader
#define SHADOW_VIEW_VERTEX		"./shaders/Shadow_view_vertex.glsl"
#define SHADOW_VIEW_FRAGMENT	"./shaders/Shadow_view_fragment.glsl"

struct MeshData
{
	mat4   m_mModelTrans;
	GLuint m_uiVAO;
	GLuint m_uiVBO;
	GLuint m_uiIBO;
	GLuint m_uiIndexCount;
};
struct Uniforms
{
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
		GLint Light_Pos;
	} View;
};

class ShadowMap : public Application
{
private:
	//
	ShaderCompiler				m_oShader;
	//
	Uniforms					m_oUniform;
	//
	mat4						m_mLightViewTrans;
	mat4						m_mLightProjTrans;
	//
	vector<MeshData>			m_oObject;
	//
	float						m_fTimer;
	//vector<tinyobj::shape_t>	m_oShapes;
	//vector<tinyobj::material_t> m_oMaterials;
	enum { RENDER_FULL, RENDER_LIGHT, RENDER_DEPTH } Mode;

	//
	GLuint						m_uiLightProgram;
	GLuint						m_uiViewProgram;
	GLint						m_uiShowLightDepthProgram;
	//
	GLuint						m_uiFBO_D; //Frame buffer depth
	GLuint						m_uiFBO_T; //Frame buffer texture
	GLuint						m_uiFBO_D_T; //Frame buffer debug texture
	GLuint						m_uiVAO_Quad; //Quad handle
	//
	bool						m_bPause;
	
public:
	ShadowMap();
	~ShadowMap();

	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void RenderPass(bool a_bFromLight);
	void OnKey();
	void LoadShader();
	void LoadMesh(const char* a_pccFileName);
	void DrawMesh(MeshData a_oMesh);
};
#endif //!_SHADOWMAP_H_