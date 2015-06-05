#include "PhysicsProject.h"

int main()
{	
	PhysicsProject* _app = new PhysicsProject(vec3(0, 10, 15), ivec2(1920, 1080), "GraphicsAssignment", false);

	//PhysicsProject* _app = new PhysicsProject();

	//_app->Init(_app, vec3(0, 10, 15), ivec2(1920, 1080), "GraphicsAssignment", false);
	
	_app->RunApp();
	
	delete _app;

	return NULL;
}

/*Other exercise main*/
//#include "Dof.h"
//
//int main()
//{
//	Dof _app;
//
//	float _dt;
//
//	_app.InitWindow();
//	
//	while (_app.IsOpen())
//	{
//		_app.ClearColor(vec4(0.2f, 0.2f, 0.2f, 0.2f));
//		_app.StartFrame();
//		_dt = (float)_app.GetDelta();
//
//		_app.Update(_dt);
//
//		_app.Draw();
//
//		_app.SwapBuffer();
//
//		_app.EndFrame();
//	}
//	_app.CleanUpWin();
//	return NULL;
//}