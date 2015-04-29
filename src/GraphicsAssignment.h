#ifndef _GRAPHICSASSIGMENT_H_
#define _GRAPHICSASSIGMENT_H_
//Lib
#include "BaseApplication.h"
#include "ShaderProgram.h"

struct SimpleFBO
{
	GLuint m_uiFBO;
	GLuint m_uiWidth;
	GLuint m_uiHeight;
	GLuint m_uiDepthColTex;
	GLuint m_uiDepthTex;

	struct TextureDesc
	{
		GLenum m_eFormat;
		GLenum m_eType;
		GLint m_iWrap;
		GLint m_iFilter;
		TextureDesc() : m_eFormat(NULL), 
						m_eType(NULL),
						m_iWrap(GL_CLAMP_TO_EDGE),
						m_iFilter(GL_NEAREST)
		{
		}
	};

	struct Descrip
	{
		GLuint m_uiW;
		GLuint m_uiH;
		TextureDesc m_oColour;
		TextureDesc m_uiDepth;
		Descrip() : m_uiW(NULL), m_uiH(NULL)
		{
		}
	};

	SimpleFBO() : m_uiFBO(NULL), 
				  m_uiWidth(NULL), 
				  m_uiHeight(NULL)
	{
	}

	~SimpleFBO()
	{
		if (m_uiDepthColTex)
		{
			glDeleteTextures(1, &m_uiDepthColTex);
			m_uiDepthColTex = NULL;
		}
		if (m_uiDepthTex)
		{
			glDeleteTextures(1, &m_uiDepthTex);
			m_uiDepthTex = NULL;
		}
		if (m_uiFBO)
		{
			glDeleteFramebuffers(1, &m_uiFBO);
			m_uiFBO = NULL;
		}
	}

	void CreateFBO(const Descrip& a_oDesc)
	{
		GLuint _prevFBO		= 0;
		m_uiDepthColTex		= NULL;
		m_uiDepthTex		= NULL;

		// Enum has MANY names based on extension/version
		// but they all map to 0x8CA6
		glGetIntegerv(0x8CA6, (GLint*)&_prevFBO);

		glGenFramebuffers(1, &m_uiFBO);

		if (a_oDesc.m_oColour.m_eFormat)
		{
			m_uiDepthColTex = CreateColTex2D(a_oDesc);
			
			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uiDepthColTex, 0);
		}

		if (a_oDesc.m_uiDepth.m_eFormat)
		{
			m_uiDepthColTex = CreateDepthTex2D(a_oDesc);

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_uiDepthTex, 0);
		}

		CheckIntegrity();
	
		m_uiWidth = a_oDesc.m_uiW;
		m_uiHeight = a_oDesc.m_uiH;

		//Restore fbo;
		glBindFramebuffer(GL_FRAMEBUFFER, _prevFBO);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO);
	}

private:
	GLuint CreateColTex2D(const Descrip& a_oDesc)
	{
		GLuint _result = NULL;

		glGenTextures(1, &_result);
		glBindTexture(GL_TEXTURE_2D, _result);
		glTexImage2D(GL_TEXTURE_2D, 
			0, 
			a_oDesc.m_oColour.m_eFormat, 
			a_oDesc.m_uiW, 
			a_oDesc.m_uiH, 
			0, 
			a_oDesc.m_oColour.m_eFormat, 
			a_oDesc.m_oColour.m_eType, 
			NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, a_oDesc.m_oColour.m_iWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, a_oDesc.m_oColour.m_iWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_oDesc.m_oColour.m_iFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_oDesc.m_oColour.m_iFilter);

		return _result;
	}

	GLuint CreateDepthTex2D(const Descrip& a_oDesc)
	{
		GLuint _result = NULL;

		glGenTextures(1, &_result);
		glBindTexture(GL_TEXTURE_2D, _result);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			a_oDesc.m_uiDepth.m_eFormat,
			a_oDesc.m_uiW,
			a_oDesc.m_uiH,
			0,
			a_oDesc.m_uiDepth.m_eFormat,
			a_oDesc.m_uiDepth.m_eType,
			NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, a_oDesc.m_uiDepth.m_iWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, a_oDesc.m_uiDepth.m_iWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_oDesc.m_uiDepth.m_iFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_oDesc.m_uiDepth.m_iFilter);

		return _result;
	}

	void CheckIntegrity()
	{
		GLuint _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		switch (_status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("<ERROR>: Can't create FBO, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("<ERROR>: Can't create FBO, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT.\n");
			break;
#if 0
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			printf("<ERROR>: Can't create FBO, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS.\n");
			break;
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("<ERROR>: Can't create FBO, GL_FRAMEBUFFER_UNSUPPORTED.\n");
			break;
		default:
			printf("<ERROR>: Can't create FBO, unknown error");
			break;
		}
	}
};

class FbxSubLoader;

class GraphicsAssignment : public App::BaseApplication
{
private:
	FbxSubLoader* m_oFbxLoader;
	FbxSubLoader* m_oFbxLoader2;
	SimpleFBO* m_oTerrainFBO;
	vec3 m_vLightDirection;
	TwBar* m_oBar;
	struct
	{
		mat4 Projection;
		mat4 View;
		mat4 VP;
		mat4 InvProj;
		mat4 InvView;
		vec4 Viewport;
		vec4 EyePosWorld;
		vec4 FrustumPlanes[6];
		vec3 LightDir;
		vec3 LightDirWorld;
		vec3 TileSize;
		vec3 GridOrigin;
		vec3 Translate;
		float Time;
		float TileBoundSphereR;
		float InvFocalLen;
		float InnerTessFactor;
		float OuterTessFactor;
		float NoiseFreq;
		float InvNoiseSize;
		float InvNoise3DSize;
		float HeightScale;
		float TriSize;
		int NoiseOctaves;
		int GridW;
		int GridH;
		bool SmoothNormal;
		bool Cull;
		bool LOD;
	} UniformTess;

	struct
	{ 
		ShaderProgram m_oTerrain;
		ShaderProgram m_oTerrainWireFrag;
		ShaderProgram m_oSkyProg;
		ShaderProgram m_oGenTerrainProg;
		ShaderProgram m_oMeshProg;
		ShaderProgram m_oMeshProg2;
	} Program;

	GLuint m_uiUBO; //Unifrom buffer.
	GLuint m_uiVBO;
	GLuint m_uiGPUQuery;
	GLuint m_uiNumOfPrim;

	GLuint m_uiNoiseTex;
	GLuint m_uiNoise3DTex;

	GLuint m_uiTerrainVBO;
	GLuint m_uiTerrainIBO;

	/* Inherit member functions */
	virtual void Render() override;
	virtual void Update(float a_fDeltaT) override;
	virtual void OnKey(GLint a_iKey, GLint a_iAction) override;
	virtual void OnMouseButton(GLint a_iButton, GLint a_iAction) override;
	virtual void OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY) override;
	virtual void OnMouseWheel(GLdouble a_dPosition) override;
	virtual void Shutdown() override;
	/* Terrain tessellation functions */
	void LoadShaders();
	void InitTerrain();
	/* Setting Unifrom */
	void InitOneTimeUniform();
	void SettingProgramsUniform();
	void SetSkyUniform();
	void SetTerrainUniform();
	void SetGenerationUniform();
	void SetWireframeUniform();
	void SetMeshUniform();
	void SetMeshUniform2();
	/* */
	void InitRendering();
	void UpdateTerrainTex();
	void FrustumPlanes(mat4& a_mView, mat4& a_mProj, vec4* a_vPlane);
	bool SphereIsFrosted(vec3 a_vPos, float a_fRay, vec4* a_vPlane);
	void DrawTerrain();
	void DrawQuad(float a_fZ);
	void DrawSky();
	void InitBars();
	void UpdateInput();
	
public:
	GraphicsAssignment();
	~GraphicsAssignment();

	virtual void Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen) override;
};

#endif //!_GRAPHICSASSIGMENT_H_