#include "GeometryProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

//Temporary shader source code, pretty basic function to determine pos, col and some uniform.
const char* _vertexSrc =
"#version 330 core \n"
"layout(location = 0) in vec4 position; \n"
"layout(location = 1) in vec4 colour; \n"
"out vec4 Colour; \n"
"uniform mat4 ProjectionView; \n"
"uniform float time; \n"
"uniform float heightScale; \n"
"uniform float noise; \n"
"void main() \n"
"{ \n"
"Colour = colour; \n"
"vec4 P = position; \n"
//"P.y = sin(time + position.x) + sin(time + position.z) * (heightScale / noise); \n"
"gl_Position = ProjectionView * P; \n"
"}";

const char* _fragSrc =
"#version 330 core \n"
"in vec4 Colour; \n"
"out vec4 OutColor; \n"
"void main() \n"
"{ \n"
"OutColor = Colour * vec4(0.2f, 0.3f, 0.8f, 1.0f); \n"
"}";

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

Camera GeometryProject::m_oCamera = Camera();
bool GeometryProject::m_bKeys[1024];
static bool m_bMouse;
static GLdouble m_fPrevX;
static GLdouble m_fPrevY;

GeometryProject::GeometryProject() : m_fTimer(NULL),
									 m_fSinAug(NULL),
									 m_fTimeAug(NULL),
									 m_uiShaderProgram(NULL),
									 m_uiVAO(NULL),
									 m_uiVBO(NULL),
									 m_uiIBO(NULL),
									 m_uiIndexCount(NULL),
									 _dt(NULL)
{

}

GeometryProject::~GeometryProject()
{

}

void GeometryProject::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vScreenSize, a_pccWinName, a_bFullScreen);

	glfwSetKeyCallback(m_oWin, key_callback);
	glfwSetCursorPosCallback(m_oWin, mouse_callback);
	glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
	glfwSetScrollCallback(m_oWin, scroll_callback);

	m_oCamera.BuildCamera(vec3(0.0f, 20.0f, 3.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	CreateShaderProgram();

	GenerateGrid(50, 50);

	GenerateCube();

	//glEnable(GL_CULL_FACE);
}

void GeometryProject::CleanUpWin()
{
	Application::CleanUpWin();
}

void GeometryProject::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	MoveCamera((float)a_fDeltaTime);

	IncreaseValue();

	m_fTimer += (float)GetDelta();
}

void GeometryProject::Draw()
{
	Use();
}

mat4 GeometryProject::BuildOrbitMatrix(float a_fLocalRot, float a_fRad, float a_fOrbitRot)
{
	mat4 _result = rotate(a_fOrbitRot, glm::vec3(0, 1, 0)) *
		translate(vec3(a_fRad, 0, 0)) *
		rotate(a_fLocalRot, vec3(0, 1, 0));

	return _result;
}

void GeometryProject::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
{
	printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void GeometryProject::MoveCamera(float a_fDeltaTime)
{
	// Camera controls
	if (m_bKeys[GLFW_KEY_W])
		m_oCamera.KeyboardInput(FORWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_S])
		m_oCamera.KeyboardInput(BACKWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_A])
		m_oCamera.KeyboardInput(LEFT, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_D])
		m_oCamera.KeyboardInput(RIGHT, a_fDeltaTime);
}

void GeometryProject::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
{
	if (m_bMouse)
	{
		m_fPrevX = (float)a_iMouseX;
		m_fPrevY = (float)a_iMouseY;
		m_bMouse = false;
	}

	GLfloat xoffset = (float)(a_iMouseX - m_fPrevX);
	GLfloat yoffset = (float)(m_fPrevY - a_iMouseY);  // Reversed since y-coordinates go from bottom to left

	m_fPrevX = (float)a_iMouseX;
	m_fPrevY = (float)a_iMouseY;

	m_oCamera.MouseInput(xoffset, yoffset);
}

void GeometryProject::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
{
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

void GeometryProject::CreateShaderProgram()
{
	GLint _succes;

	GLuint _vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint _fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(_vertexShader, 1, &_vertexSrc, NULL);
	glCompileShader(_vertexShader);

	glShaderSource(_fragShader, 1, &_fragSrc, NULL);
	glCompileShader(_fragShader);

	m_uiShaderProgram = glCreateProgram();
	glAttachShader(m_uiShaderProgram, _vertexShader);
	glAttachShader(m_uiShaderProgram, _fragShader);

	glLinkProgram(m_uiShaderProgram);

	glGetProgramiv(m_uiShaderProgram, GL_LINK_STATUS, &_succes);

	if (!_succes)
	{
		GLint _linkInfoLgh = 0;

		glGetProgramiv(m_uiShaderProgram, GL_INFO_LOG_LENGTH, &_linkInfoLgh);

		GLchar* _infoLog = new GLchar[_linkInfoLgh];

		glGetProgramInfoLog(m_uiShaderProgram, _linkInfoLgh, NULL, _infoLog);

		printf("Error: <Failed to link shader source code to the shader program.> %s\n", _infoLog);
		delete[] _infoLog;
	}

	glDeleteShader(_fragShader);
	glDeleteShader(_vertexShader);
}

void GeometryProject::Use()
{
	glUseProgram(m_uiShaderProgram);

	GLuint _projViewUni = glGetUniformLocation(m_uiShaderProgram, "ProjectionView");
	mat4 _projview = m_oProjection * m_oView;
	glUniformMatrix4fv(_projViewUni, 1, GL_FALSE, glm::value_ptr(_projview));

	GLint _timeUni = glGetUniformLocation(m_uiShaderProgram, "time");

	_dt += ((float)GetDelta() * m_fTimeAug);
	glUniform1f(_timeUni, _dt);

	GLuint _perturbanceHeight = glGetUniformLocation(m_uiShaderProgram, "heightScale");
	glUniform1f(_perturbanceHeight, m_fSinAug);

	vec3 _p = vec3(152.0f, 152.0f, 0.0f);

	vec3 _i = floor(_p);
	vec3 _f = glm::fract(_p);
	vec3 _u = _f * _f * (3.0f - 2.0f * _f);

	float _resultNoise = -1.0f + 2.0f * glm::mix(glm::mix(Hash(_i + vec3(0, 0, 0)), Hash(_i + vec3(1.0f, 0.0f, 0.0f)), _u.x),
		glm::mix(Hash(_i + vec3(0.0f, 1.0f, 0.0f)), Hash(_i + vec3(1.0f, 1.0f, 0.0f)), _u.x), _u.y);

	GLuint _noise = glGetUniformLocation(m_uiShaderProgram, "noise");
	glUniform1f(_noise, _resultNoise);

	glBindVertexArray(m_uiVAO);
	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}

void GeometryProject::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
{
	//For better readbility
	unsigned int _count = a_uiRows * a_uiCols;
	unsigned int* _auiIndices = new unsigned int[(a_uiRows - 1) * (a_uiCols - 1) * 6];
	m_uiIndexCount = (a_uiRows - 1) * (a_uiCols - 1) * 6;
	//Create new vertex to allocate position and colours (ATM just that, later we can do more stuff, UV, normal ecc).
	Vertex* _vertices = new Vertex[(a_uiRows + 1)*(a_uiCols + 1)];
	//Gridding the new vertex to position.
	for (unsigned int rw = 0; rw < a_uiRows + 1; ++rw)
	{
		for (unsigned int cl = 0; cl < a_uiCols + 1; ++cl)
		{
			_vertices[rw * a_uiCols + cl].m_vPos = vec4((float)cl, 0.0f, (float)rw, 1);
			//float _x = ((rw - a_uiCols / 2.0f) / (a_uiCols / 2.0f));
			//float _y = ((cl - a_uiCols / 2.0f) / (a_uiCols / 2.0f));
			//float _d = hypotf(_x, _y) * 0.2f;
			//float _z = sinf(sin(0.5f - _d * _d) * expf(_d * _d / -2.0f)) / tanf(0.5f * 34 / a_uiCols);
			//_z = roundf(_z * 27 + 18);
			//_vertices[rw * a_uiCols + cl].m_vPos = vec4(_x, _y, _z, 1.0f);

			vec3 _colour = vec3(cosf((cl / (float)(a_uiCols - 1)) * (rw / (float)(a_uiRows - 1))));
			_vertices[rw * a_uiCols + cl].m_vCol = vec4(_colour, 1.0f);
		}
	}
	//Assign the indices to determine where to draw our triangles on screen.
	unsigned int _index = 0;
	for (unsigned int rw = 0; rw < (a_uiRows - 1); ++rw)
	{
		for (unsigned int cl = 0; cl < (a_uiCols - 1); ++cl)
		{
			// triangle 1
			_auiIndices[_index++] = rw * a_uiCols + cl;
			_auiIndices[_index++] = (rw + 1) * a_uiCols + cl;
			_auiIndices[_index++] = (rw + 1) * a_uiCols + (cl + 1);
			// triangle 2
			_auiIndices[_index++] = rw * a_uiCols + cl;
			_auiIndices[_index++] = (rw + 1) * a_uiCols + (cl + 1);
			_auiIndices[_index++] = rw * a_uiCols + (cl + 1);
		}
	}
	//Generate both the vertex buffer and the index buffer object
	glGenBuffers(1, &m_uiVBO);
	glGenBuffers(1, &m_uiIBO);
	//Now it's time to generate the vertex array buffer
	glGenVertexArrays(1, &m_uiVAO);
	//The new buffer need to be binded
	glBindVertexArray(m_uiVAO);
	//Now we bind the other two buffers 
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	//VBO data buffering
	glBufferData(GL_ARRAY_BUFFER, _count * sizeof(Vertex), _vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//IBO data buffering
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (a_uiRows - 1) * (a_uiCols - 1) * 6 * sizeof(unsigned int), _auiIndices, GL_DYNAMIC_DRAW);
	//And we wrap de-binding the buffers.
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Let's clean the vertices create a moment ago to avoid memory leaking.
	delete[] _vertices;
	delete[] _auiIndices;
}

void GeometryProject::GenerateCube()
{
	Vertex* _cubeVertex = new Vertex[8];

	for (unsigned int i = 0; i < 8; ++i)
	{
		switch (i)
		{
			//Front
		case 0:
			_cubeVertex[i].m_vPos = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 1:
			_cubeVertex[i].m_vPos = vec4(1.0f, -1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 2:
			_cubeVertex[i].m_vPos = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 3:
			_cubeVertex[i].m_vPos = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
			//Back
		case 4:
			_cubeVertex[i].m_vPos = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 5:
			_cubeVertex[i].m_vPos = vec4(1.0f, -1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 6:
			_cubeVertex[i].m_vPos = vec4(1.0f, 1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 7:
			_cubeVertex[i].m_vPos = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		default:
			break;
		}
	}
	GLuint _cubeIndex[] =
	{
		//Front
		0, 1, 2,
		2, 3, 0,
		//Top
		3, 2, 6,
		6, 7, 3,
		//Back
		7, 6, 5,
		5, 4, 7,
		//Bottom
		4, 5, 1,
		1, 0, 4,
		//Left
		4, 0, 3,
		3, 7, 4,
		//Right
		1, 5, 6,
		6, 2, 1
	};
	//Generate both the vertex buffer and the index buffer object
	glGenBuffers(1, &m_uiVBO);
	glGenBuffers(1, &m_uiIBO);
	//Now it's time to generate the vertex array buffer
	glGenVertexArrays(1, &m_uiVAO);
	//The new buffer need to be binded
	glBindVertexArray(m_uiVAO);
	//Now we bind the other two buffers 
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	//VBO data buffering
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), _cubeVertex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//IBO data buffering
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_cubeIndex), _cubeIndex, GL_STATIC_DRAW);
	//And we wrap de-binding the buffers.
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Let's clean the vertices create a moment ago to avoid memory leaking.
	delete[] _cubeVertex;
	//delete[] _cubeIndex;
}

void GeometryProject::IncreaseValue()
{
	if (m_bKeys[GLFW_KEY_PAGE_UP])
		m_fSinAug += 0.1f;
	if (m_bKeys[GLFW_KEY_PAGE_DOWN])
		m_fSinAug -= 0.1f;
	if (m_bKeys[GLFW_KEY_HOME])
		m_fTimeAug++;
	if (m_bKeys[GLFW_KEY_END])
		m_fTimeAug--;
}

float GeometryProject::Hash(vec3 _p)
{
	float _h = glm::dot(_p, vec3(127.1, 311.7, 0.0f));

	return glm::fract(sin(_h) * 43758.5453123f);
}