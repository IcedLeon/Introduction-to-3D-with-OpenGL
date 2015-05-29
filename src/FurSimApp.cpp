#include "FurSimApp.h"
#include "TextureLoader.h"

#define LOG(_log, ...) { printf(_log, __VA_ARGS__); }

FurSim::FurSim()
{

}

FurSim::~FurSim()
{

}

//void CreateNoise

void FurSim::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	m_oMyLights = { { vec3(1.0f, 1.0f, 1.0f) }, { vec4(0.3f, 0.3f, 0.3f, 1.0f) }, { vec4(1.0f) }, { vec4(1.0f) } };
	
	DATA.TRANSFORM.m_mProjection = DATA.m_oCurrCamera->GetProjectionTransform(APPINFO.m_viWinSize.xy(), 1.0f, 10000.0f);
	
	m_uiTigerTex = TEXLOADER::LoadTexture("./textures/tiger.tga", false, true);


}

void FurSim::LoadShaders()
{
	vector<const char*> _shrSource;
	
	LOG("Compiling ambient diffuse texture shaders...");
	_shrSource.push_back(AMBIENT_VERT);
	_shrSource.push_back(AMBIENT_FRAG);
	m_oProgram.CreateProgram(_shrSource);
	_shrSource.clear();

	LOG("Compiling fur simulation shaders...");
	_shrSource.push_back(FUR_VERTEX);
	_shrSource.push_back(FUR_GEOM);
	_shrSource.push_back(FUR_FRAG);
	m_oFurProgram.CreateProgram(_shrSource);
	_shrSource.clear();
}

void FurSim::InitUniform()
{
	m_oProgram.Use();
	const char* _uniformNames[] =
	{
		"u_projectionMatrix",
		"u_modelViewMatrix",
		"u_normalMatrix",
		"u_light.direction",
		"u_light.ambientColor",
		"u_light.diffuseColor",
		"u_light.specularColor",
		"u_textureFurColor",
		"a_vertex",
		"a_normal",
		"a_texCoord"
	};

	for (unsigned int i = 0; i < 11; ++i)
	{
		switch (i)
		{
		case 0:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 1:
			//m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m);
			break;
		case 2:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 3:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 4:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 5:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 6:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 7:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 8:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 9:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		case 10:
			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
			break;
		}
	}
}

void FurSim::Render()
{

}

void FurSim::Update(float a_fDeltaT)
{

}

void FurSim::OnResize(GLint a_iWidth, GLint a_iHeight)
{

}

void FurSim::OnKey(GLint a_iKey, GLint a_iAction)
{

}

void FurSim::OnMouseButton(GLint a_iButton, GLint a_iAction)
{

}

void FurSim::OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY)
{

}

void FurSim::OnMouseWheel(GLdouble a_dPosition)
{

}

void FurSim::Shutdown()
{

}