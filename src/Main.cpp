#include "GeometryProject.h"

int main()
{
	GeometryProject _app;

	float _dt;

	_app.InitWindow();

	while (_app.IsOpen())
	{
		_app.ClearColor(vec4(0.2f, 0.2f, 0.2f, 0.2f));
		_app.StartFrame();
		_dt = (float)_app.GetDelta();

		_app.Update(_dt);

		_app.GenerateGrid(50, 50);

		_app.Draw();

		_app.SwapBuffer();

		_app.EndFrame();
	}
	_app.CleanUpWin();
	return NULL;
}