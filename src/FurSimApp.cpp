#include "FurSimApp.h"

FurSim::FurSim()
{

}

FurSim::~FurSim()
{

}

void FurSim::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	m_oMyLights = { { vec3(1.0f, 1.0f, 1.0f) }, { vec4(0.3f, 0.3f, 0.3f, 1.0f) }, { vec4(1.0f) }, { vec4(1.0f) } };
	
	
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