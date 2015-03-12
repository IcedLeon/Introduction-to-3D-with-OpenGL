#ifndef _THREADPROJECT_H_
#define _THREADPROJECT_H_
//Lib
#include "Application.h"
#include <thread>

std::thread my_thread;

class ThreadProject : public Application
{
private:

public:
	ThreadProject();
	~ThreadProject();
	
	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, 0.0f, 4.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();
	
};
#endif //!_THREADPROJECT_H_