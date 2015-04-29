#ifndef _BASEAPPLICATION_H_
#define _BASEAPPLICATION_H_
//Lib
#include "BaseInclude.h"
//GLFW
#include <GLFW\glfw3.h>
#include "Camera.h"
#include <AntTweakBar.h>

#ifdef _DEBUG
#endif

class Bar
{
private:
	//Map of the tweek bars
	map<const char*, TwBar*> m_mpTweekBar;
	template <typename T>
	struct TypeToTW
	{
		static const TwType value{ TW_TYPE_UNDEF };
	};

	template <>	struct TypeToTW<int> { static const TwType value{ TW_TYPE_INT32 }; };
	template <>	struct TypeToTW<bool> { static const TwType value{ TW_TYPE_BOOLCPP }; };
	template <>	struct TypeToTW<float> { static const TwType value{ TW_TYPE_FLOAT }; };
	template <>	struct TypeToTW<double> { static const TwType value{ TW_TYPE_DOUBLE }; };

public:
	Bar() {}
	~Bar() {}

	//AntTweakBar initialise funcs
	void InitTweek();

	void CreateBar(const char* a_sNewBarName);
	TwBar* GetMappedBar(const char* a_sBarName);

	void DrawTweek();
	void CleanUpTweek();
	void ScaleTweek(int a_iWidth, int a_iHeight);

	template<typename T>
	inline void AddR_WTweak(const char* a_pccDivisor, const char* a_pccName, T a_Var, const char* a_pccDefinition)
	{
		TwBar* _currBar = m_mpTweekBar[a_pccDivisor];
		if (!TwAddVarRW(_currBar, a_pccName, TypeToTW<T>::value, &a_Var, a_pccDefinition))
		{
			printf("This might be an error... fuck this AntTweakBar thingy!");
		}
		else
		{
			printf("OK it might be somewhere else...");
		}
	}
	template<typename T>
	inline void AddR_OVar(const char* a_pccDivisor, const char* a_pccName, T a_Var, const char* a_pccDefinition)
	{
		TwAddVarRO(m_mpTweekBar[a_pccDivisor], a_pccName, TypeToTW<T>::value, &a_Var, a_pccDefinition);
	}

	void AddTweakColor3f(const char* a_pccDivisor, const char* a_pccName, vec3 a_vCol, const char* a_pccDefinition);
	void AddTweakColor4f(const char* a_pccDivisor, const char* a_pccName, vec4 a_vCol, const char* a_pccDefinition);

	void AddTweakDir3f(const char* a_pccDivisor, const char* a_pccName, vec3 a_vDir, const char* a_pccDefinition);
};

namespace App
{
	class BaseApplication
	{
	public:
		struct
		{
			GLFWwindow*			m_oWin;
			Camera*				m_oCurrCamera;
			Bar*				m_oTweeking;
			map<int, Camera*>	m_oTotalCameras;
			struct
			{
				mat4 m_mProjection;
				mat4 m_mView;
				mat4 m_mWorld;
			} TRANSFORM;
			struct
			{
				GLdouble m_dDeltaT;
				GLdouble m_dCurrT;
				GLdouble m_dPrevT;
			} TIME;
		} DATA;
		struct 
		{
			const char* m_pccTitle;
			ivec2		m_viWinSize;
			ivec2		m_viGLVersion; //X-> Major ver, Y->Minor ver.
			int			m_iSample;
			union
			{
				struct
				{
					unsigned int m_uiFullScreen : 1;
					unsigned int m_uiVSync		: 1;
					unsigned int m_uiCursor		: 1;
					unsigned int m_uiDebug		: 1;
				};
				unsigned int m_uiAll;
			} Flags;
		} APPINFO;
		static bool	m_bKeys[MAX_KEYS];

	protected:
		static App::BaseApplication* m_oApp;
		static void framebuffer_size_callback(GLFWwindow* a_oWindow, GLint a_iWidth, GLint a_iHeight);
		static void key_callback(GLFWwindow* a_oWindow, GLint a_iKey, GLint a_iKeyCode, GLint a_iAction, GLint a_iMode);
		static void mouse_callback(GLFWwindow* a_oWindow, GLdouble a_iMouseX, GLdouble a_iMouseY);
		static void mouse_button_callback(GLFWwindow* a_oWindow, GLint a_iButton, GLint a_iAction, GLint a_iMode);
		static void scroll_callback(GLFWwindow* a_oWindow, GLdouble a_fOffsetX, GLdouble a_fOffsetY);
		static void on_char_callback(GLFWwindow* a_oWindow, GLuint a_uiCodepoint, GLint a_iMode);
		static void APIENTRY debug_callback(GLenum a_eSource, GLenum a_eType, GLuint a_uiID, 
			GLenum a_eSeverity, GLsizei a_siLength, const GLchar* a_pccMsg, const GLvoid* a_pcvParam);
		//static void APIENTRY debug_callback_AMD(GLenum a_eType, GLuint a_uiID, 
		//	GLenum a_eSeverity, GLsizei a_siLength, const GLchar* a_pccMsg, const GLvoid* a_pcvParam);

		void InitWindow();

		void ChooseResolution();

		void InitDependency(vec3 a_vCamPos);

		void DumpGLInfo();

		void CreateCamera(vec3 a_vCamPos);

		void SwitchCamera(int a_iCamID);

		virtual void Render() = NULL;

		virtual void Update(float a_fDeltaT) = NULL;

		virtual void OnResize(GLint a_iWidth, GLint i_iHeight);
		
		virtual void OnKey(GLint a_iKey, GLint a_iAction) = NULL;

		virtual void OnMouseButton(GLint a_iButton, GLint a_iAction) = NULL;

		virtual void OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY) = NULL;

		virtual void OnMouseWheel(GLdouble a_dPosition) = NULL;

		virtual void GetGLError(GLenum a_eSource, GLenum a_eType, GLuint a_uiID,
			GLenum a_eSeverity, const GLchar* a_pccMsg);

		virtual void Shutdown();

		void EnableVsync(bool a_bVsyncEnable);
		
	public:
		BaseApplication();
		BaseApplication(const BaseApplication& a_Rhs);
		~BaseApplication();

		virtual void Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen);
		
		void RunApp();

	};
}
//
//#define DECLARE_MAIN(a)								\
//App::BaseApplication* _newApp = NULL;				\
//int CALLBACK WinMain(HINSTANCE _hInstance,			\
//					 HINSTANCE _hPrevInstance,		\
//					 LPSTR lpCmdLide,				\
//					 int nCmdShow)					\
//{													\
//	a* _newApp = new a;								\
//	_newApp->RunApp();								\
//	delete _newApp;									\
//	return NULL;									\
//}

#endif //!_BASEAPPLICATION_H_