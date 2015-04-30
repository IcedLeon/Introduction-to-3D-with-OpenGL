//#include "GameTree.h"
//
//int main()
//{	
//	GameTree* _app = new GameTree();
//
//	_app->Init(_app, vec3(0, 10, 15), ivec2(1024, 720), "GraphicsAssignment", false);
//	
//	_app->RunApp();
//	
//	delete _app;
//
//	return NULL;
//}

/*Other exercise main*/
#include "GameTree.h"

int main()
{
	GameTree _app;

	float _dt;

	_app.InitWindow();
	
	while (_app.IsOpen())
	{
		_app.ClearColor(vec4(0.2f, 0.2f, 0.2f, 0.2f));
		_app.StartFrame();
		_dt = (float)_app.GetDelta();

		_app.Update(_dt);

		_app.Draw();

		_app.SwapBuffer();

		_app.EndFrame();
	}
	_app.CleanUpWin();
	return NULL;
}