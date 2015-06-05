#ifndef _PHYSICSPROJECT_H_
#define _PHYSICSPROJECT_H_
//Lib
#include "BaseApplication.h"

class PhysicsProject : public App::BaseApplication
{
private:

	/* Inherit member functions */
	virtual void Render() override;
	virtual void Update(float a_fDeltaT) override;
	virtual void OnKey(GLint a_iKey, GLint a_iAction) override;
	virtual void OnMouseButton(GLint a_iButton, GLint a_iAction) override;
	virtual void OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY) override;
	virtual void OnMouseWheel(GLdouble a_dPosition) override;
	virtual void Shutdown() override;

	void UpdateInput();

public:
	PhysicsProject() { }
	PhysicsProject(vec3 a_vCamPos, ivec2 a_vScreenSize, 
		const char* a_pccWinName, bool a_bFullScreen) { Init(this, a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen); }
	~PhysicsProject() { }

	virtual void Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen) override;
};

#endif //!_PHYSICSPROJECT_H_