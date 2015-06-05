#include "PhysicsProject.h"


void PhysicsProject::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	App::BaseApplication::Init(a_oCurrApp, a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	/* Projection */
	DATA.TRANSFORM.m_mProjection = DATA.m_oCurrCamera->GetProjectionTransform(APPINFO.m_viWinSize.xy(), 1.0f, 10000.0f);

}

void PhysicsProject::Render()
{
	glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
}

void PhysicsProject::Update(float a_fDeltaT)
{
	/* View */
	DATA.TRANSFORM.m_mView = DATA.m_oCurrCamera->GetViewTransform();
	/* Model */
	DATA.TRANSFORM.m_mWorld = DATA.m_oCurrCamera->GetWorldTransform();
}

void PhysicsProject::UpdateInput()
{
	/* Camera controls */
	if (m_bKeys[GLFW_KEY_W])
		DATA.m_oCurrCamera->KeyboardInput(FORWARD, (float)DATA.TIME.m_dDeltaT);
	if (m_bKeys[GLFW_KEY_S])
		DATA.m_oCurrCamera->KeyboardInput(BACKWARD, (float)DATA.TIME.m_dDeltaT);
	if (m_bKeys[GLFW_KEY_A])
		DATA.m_oCurrCamera->KeyboardInput(LEFT, (float)DATA.TIME.m_dDeltaT);
	if (m_bKeys[GLFW_KEY_D])
		DATA.m_oCurrCamera->KeyboardInput(RIGHT, (float)DATA.TIME.m_dDeltaT);
	if (m_bKeys[GLFW_KEY_R])
		//LoadShaders();

	///* Wire mode off */
	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/* Wire mode on */
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void PhysicsProject::OnKey(GLint a_iKey, GLint a_iAction)
{
	TwEventKeyGLFW(a_iKey, a_iAction);
}

void PhysicsProject::OnMouseButton(GLint a_iButton, GLint a_iAction)
{
	TwEventMouseButtonGLFW(a_iButton, a_iAction);
}

void PhysicsProject::OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY)
{
	TwEventMousePosGLFW((int)a_dMouseX, (int)a_dMouseY);
}

void PhysicsProject::OnMouseWheel(GLdouble a_dPosition)
{
	TwEventMouseWheelGLFW((int)a_dPosition);
}

void PhysicsProject::Shutdown()
{

}