//#include "gl_core_4_4.h"
//#include <GLFW\glfw3.h>
//#include "Gizmos.h"
//
//#define GLM_SWIZZLE
//#include "glm/glm.hpp"
//#include "glm/ext.hpp"
//
//#include <thread>
//
//#include "Application.h"
//#include "Camera.h"
//
//#define WIN_NAME_PREFIX "OPENGL TEST"
//
//GLFWwindow* _win;
//int _width,
//	_height;
//
//GLfloat _dt;
//GLfloat _prevTime;
//GLfloat _currTime;
//
//void StartWindow();
//void SwapBuff();
//void CleanUpWindow();
//
//void StartFrame();
//
//void EndFrame();
//
////glm::mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot)
////{
////	glm::mat4 _result = glm::rotate(a_fOrbitRot, glm::vec3(0, 1, 0) ) * 
//// glm::translate(vec3(a_fRad, 0, 0)) *
//// glm::rotate(a_fLocalRot, vec3(0, 1, 0));
////}
//
//int main()
//{
//	StartWindow();
//
//	glClearColor(0.5f, 0.3f, 0.6f, 0.2f);
//
//	float _cameraX = -5.0f;
//	float _cameraZ = -10.0f;
//	float _timer = 0.0f;
//
//	Camera* _camera = new Camera();
//
//	_camera->BuildCamera(glm::vec3(0, 0, 3.0f));
//
//	Gizmos::create();
//
//	glm::mat4 _proj = glm::perspective((float)glm::radians(60.0f), (float)_width / _height, 0.1f, 1000.0f);
//
//	while (!glfwWindowShouldClose(_win))
//	{
//		StartFrame();
//		glm::mat4 _view = glm::lookAt(glm::vec3(_cameraX, 10.0f, _cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		_timer += _dt;
//		//_cameraX = sin(_timer) * 10;
//		//_cameraZ = cos(_timer) * 10;
//
//		if (glfwGetKey(_win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		{
//			CleanUpWindow();
//		}
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		Gizmos::clear();
//
//		Gizmos::addTransform(glm::mat4(1));
//
//		glm::vec4 _white(1);
//		glm::vec4 _black(0, 0, 0, 1);
//		glm::vec4 _blue(0, 0, 1, 1);
//		glm::vec4 _green(0, 1, 0, 1);
//		glm::vec4 _red(1, 0, 0, 1);
//
//
//		for (int i = 0; i <= 20; ++i)
//		{
//			Gizmos::addLine(glm::vec3(-10 + i, 0, -10),
//				glm::vec3(-10 + i, 0, 10),
//				i == 10 ? _white : _black);
//			Gizmos::addLine(glm::vec3(-10, 0, -10 + i),
//				glm::vec3(10, 0, -10 + i), 
//				i == 10 ? _white : _black);
//		}
//
//
//		glm::mat4 _rotMat = glm::rotate(_timer, glm::vec3(0, 1, 0));
//
//		glm::mat4 _transMatChild = _rotMat * glm::translate(glm::vec3(5, 0, 0)) * glm::rotate(_timer * 2.0f, glm::vec3(0, 1, 0) );
//
//		glm::mat4 _transMatChild2 = _transMatChild * glm::translate(glm::vec3(1, 0, 0)) * glm::rotate(_timer * 4.0f, glm::vec3(0, 1, 0));
//
//		glm::mat4 _minecraftPlanet = _rotMat * glm::translate(glm::vec3(9, 2, 0)) * glm::rotate(_timer * 0.7f, glm::vec3(1, 0, 1));
//
//		glm::mat4 _haloMatrix = _minecraftPlanet * glm::translate(glm::vec3(0, 0, 0)) * glm::rotate(_timer * 0.3f, glm::vec3(0, 1, 0));
//
//		Gizmos::addSphere(_rotMat[3].xyz, 1.0f, 30, 30, _red, &_rotMat);
//
//		Gizmos::addSphere(_transMatChild[3].xyz, 0.5f, 30, 30, _green, &_transMatChild);
//		Gizmos::addSphere(_transMatChild2[3].xyz, 0.2f, 30, 30, _blue, &_transMatChild2);
//
//		Gizmos::addAABBFilled(_minecraftPlanet[3].xyz, glm::vec3(0.3, 0.3, 0.3), _red);
//
//		Gizmos::addRing(_haloMatrix[3].xyz, 1.0f, 1.5f, 20, _red, &_haloMatrix);
//
//		//for (unsigned int i = 0; i < 5; ++i)
//		//{
//		//	Gizmos::addSphere(_haloMatrix[3].xyz, 0.3f, 30, 30, _blue, &_haloMatrix);
//		//}
//
//		//Gizmos::addTri(glm::vec3(0, 1, 0), glm::vec3(2, -1, 1), glm::vec3(-3, -2, 4), _green);
//
//		//Gizmos::addAABBFilled(glm::vec3(2, 5, 0), glm::vec3(1, 1, 1), _blue);
//
//		//Gizmos::addAABBFilled(glm::vec3(-2, 5, 0), glm::vec3(1, 1, 1), _blue);
//
//		Gizmos::draw(_proj, _view);
//
//		SwapBuff();
//		
//		EndFrame();
//	}
//
//	Gizmos::destroy();
//	CleanUpWindow();
//	return NULL;
//}
//
//void StartFrame()
//{
//	_currTime = glfwGetTime();
//	_dt = _currTime - _prevTime;
//}
//
//void EndFrame()
//{
//	_prevTime = _currTime;
//}
//
////glm::mat4 BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot)
////{
////	glm::mat4 _result = glm::rotate(a_fOrbitRot, glm::vec3())
////}
//
//void StartWindow()
//{
//	if (!glfwInit())
//	{
//		printf("ERROR: <CANNOT INITIALIZE A WINDOW. glfwInit() has fail. %s\n");
//	}
//	else
//	{
//		_width = 1280;
//		_height = 720;
//
//		_win = glfwCreateWindow(_width, _height, WIN_NAME_PREFIX, nullptr, nullptr);
//	}
//
//	if (!_win)
//	{
//		glfwTerminate();
//		printf("Window could not be intialize, glfwCreateWindow has fail. %s\n");
//		exit(EXIT_FAILURE);
//	}
//	else
//	{
//		glfwMakeContextCurrent(_win);
//
//		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
//		{
//			CleanUpWindow();
//		}
//		else
//		{
//			int _majorVer,
//				_minorVer;
//
//			_majorVer = ogl_GetMajorVersion();
//			_minorVer = ogl_GetMinorVersion();
//
//			printf("Successfully loaded this version of OpenGL %d %d \n", _majorVer, _minorVer);
//
//			const GLubyte* _renderer = glGetString(GL_RENDERER); // get renderer string
//			const GLubyte* _version = glGetString(GL_VERSION); // version as a string
//			printf("Renderer: %s\n", _renderer);
//			printf("OpenGL version supported %s\n", _version);
//			glEnable(GL_DEPTH_TEST);
//		}
//	}
//}
//
//void SwapBuff()
//{
//	glfwSwapBuffers(_win);
//	glfwPollEvents();
//}
//
//void CleanUpWindow()
//{
//	glfwDestroyWindow(_win);
//	glfwTerminate();
//}
