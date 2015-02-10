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
"void main() \n"
"{ \n"
"Colour = colour; \n"
"vec4 P = position; \n"
"P.y += atan(time + position.x) * sin(time + position.x) * heightScale; \n"
"gl_Position = ProjectionView * P; \n"
"}";

//"P.x += sin(time + position.y) * heightScale; \n"

const char* _fragSrc =
"#version 330 core \n"
"in vec4 Colour; \n"
"out vec4 OutColor; \n"
"void main() \n"
"{ \n"
"OutColor = Colour; \n"
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

void GeometryProject::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vScreenSize, a_pccWinName, a_bFullScreen);

	glViewport(0, 0, (GLsizei)a_vScreenSize.x, (GLsizei)a_vScreenSize.y);

	glfwSetKeyCallback(m_oWin, key_callback);
	glfwSetCursorPosCallback(m_oWin, mouse_callback);
	glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
	glfwSetScrollCallback(m_oWin, scroll_callback);

	m_oCamera.BuildCamera(vec3(0.0f, 0.0f, 3.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	CreateShaderProgram();
}

void GeometryProject::CleanUpWin()
{
	Application::CleanUpWin();
}

void GeometryProject::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();

	MoveCamera((float)a_fDeltaTime);

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

void GeometryProject::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
{
	//For better readbility
	unsigned int _count = a_uiRows * a_uiCols; 
	unsigned int* _auiIndices = new unsigned int[(a_uiRows - 1) * (a_uiCols - 1) * 6];
	m_uiIndexCount = (a_uiRows - 1) * (a_uiCols - 1) * 6;
	//Create new vertex to allocate position and colours (ATM just that, later we can do more stuff, UV, normal ecc).
	Vertex* _vertices = new Vertex[a_uiRows*a_uiCols];
	//Gridding the new vertex to position.
	for (unsigned int rw = 0; rw < a_uiRows; ++rw)
	{
		for (unsigned int cl = 0; cl < a_uiCols; ++cl)
		{
			_vertices[rw * a_uiCols + cl].m_vPos = vec4((float)cl, 0.0f, (float)rw, 1);
			vec3 _colour = vec3(sinf( (cl / (float)(a_uiCols - 1) ) * (rw / (float)(a_uiRows - 1) ) ) );
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (a_uiRows - 1) * (a_uiCols - 1) * 6 * sizeof(unsigned int), _auiIndices, GL_STATIC_DRAW);
	//And we wrap de-binding the buffers.
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Let's clean the vertices create a moment ago to avoid memory leaking.
	delete[] _vertices;
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
	glUniformMatrix4fv(_projViewUni, 1, GL_FALSE, glm::value_ptr(m_oProjection * m_oView));

	GLint _timeUni = glGetUniformLocation(m_uiShaderProgram, "time");
	float _dt = (float)GetDelta() * 1000;
	glUniform1f(_timeUni, _dt);
	
	GLuint _perturbanceHeight = glGetUniformLocation(m_uiShaderProgram, "heightScale");
	glUniform1f(_perturbanceHeight, 0.5f);

	glBindVertexArray(m_uiVAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}