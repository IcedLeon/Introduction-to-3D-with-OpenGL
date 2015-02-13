#include "AdvanceTexture.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

Camera AdvanceTexture::m_oCamera = Camera();
bool AdvanceTexture::m_bKeys[1024];
static bool m_bMouse;
static GLdouble m_fPrevX;
static GLdouble m_fPrevY;

AdvanceTexture::AdvanceTexture() : m_fTimer(NULL),
m_fSinAug(NULL),
m_fTimeAug(NULL),
m_uiIndexCount(NULL),
m_fModRGB(1)
{

}

AdvanceTexture::~AdvanceTexture()
{

}

void AdvanceTexture::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vScreenSize, a_pccWinName, a_bFullScreen);

	glfwSetKeyCallback(m_oWin, key_callback);
	glfwSetCursorPosCallback(m_oWin, mouse_callback);
	glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
	glfwSetScrollCallback(m_oWin, scroll_callback);

	m_oCamera.BuildCamera(vec3(0.0f, 20.0f, 3.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	m_oShader.CreateShaderProgram(VERTEX_SHD_CODE, FRAGMENT_SHD_CODE);

	ambient_light = vec3(0.2f);
	light_direction = vec3(0, 1, 0);
	light_colour = vec3(0.7f);
	specular_power = 15.0f;

	LoadTexture();

	GenerateQuad(5.0f);
}

void AdvanceTexture::CleanUpWin()
{
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void AdvanceTexture::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	MoveCamera((float)a_fDeltaTime);

	light_direction = (glm::rotate((float)a_fDeltaTime, vec3(0, 1, 0) ) * vec4(light_direction, 0)).xyz;

	IncreaseValue();

	m_fTimer += (float)GetDelta() * m_fTimeAug;
}

void AdvanceTexture::Draw()
{
	Use();
}

void AdvanceTexture::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
{
	printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void AdvanceTexture::MoveCamera(float a_fDeltaTime)
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

void AdvanceTexture::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
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

void AdvanceTexture::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
{
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

void AdvanceTexture::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	GLuint _ambientUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "ambient_light");
	glUniform3fv(_ambientUni, 1, (float*)&ambient_light);

	GLuint _lightDirUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_direction");
	glUniform3fv(_lightDirUni, 1, (float*)&light_direction);

	GLuint _lightColUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_colour");
	glUniform3fv(_lightColUni, 1, (float*)&light_colour);

	GLuint _eyeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "eye_pos");
	glUniform3fv(_eyeUni, 1, (float*)&m_oCamera.GetWorldTransform()[2].xyz);

	GLuint _powerUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_power");
	glUniform1f(_powerUni, specular_power);



	////texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiDiffuseTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_uiSpecularTexture);

	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "diffuse_tex");
	glUniform1i(_textDiffUni, 0);
	GLuint _textNormalUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "normal_tex");
	glUniform1i(_textNormalUni, 1);
	GLuint _textSpecUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_tex");
	glUniform1i(_textSpecUni, 2);



	//GLuint _modRGBUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "modColor");
	//glUniform1f(_modRGBUni, m_fModRGB);

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glBindVertexArray(m_oQuad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_oQuad.m_uiIndexCount, GL_UNSIGNED_INT, 0);

}

void AdvanceTexture::GenerateQuad(float a_fSize)
{
	VertexNormal _vertexData[4];
	int _index = 0;

	_vertexData[_index++].position = vec4(-a_fSize, 0.0f, -a_fSize, 1);
	_vertexData[_index++].position = vec4(-a_fSize, 0.0f, a_fSize, 1);
	_vertexData[_index++].position = vec4(a_fSize, 0.0f, a_fSize, 1);
	_vertexData[_index++].position = vec4(a_fSize, 0.0f, -a_fSize, 1); 
	
	_index = 0;

	_vertexData[_index++].normal = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	_vertexData[_index++].normal = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	_vertexData[_index++].normal = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	_vertexData[_index++].normal = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	_index = 0;

	_vertexData[_index++].tangent = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	_vertexData[_index++].tangent = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	_vertexData[_index++].tangent = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	_vertexData[_index++].tangent = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	_index = 0;

	_vertexData[_index++].text_coord = vec2(0, 0);
	_vertexData[_index++].text_coord = vec2(0, 1);
	_vertexData[_index++].text_coord = vec2(1, 1);
	_vertexData[_index++].text_coord = vec2(1, 0);

	GLuint _quadIndex[6] = { 0, 1, 2, 0, 2, 3 };
	m_oQuad.m_uiIndexCount = 6;

	//Generate both the vertex buffer and the index buffer object
	glGenBuffers(1, &m_oQuad.m_uiVBO);
	glGenBuffers(1, &m_oQuad.m_uiIBO);
	//Now it's time to generate the vertex array buffer
	glGenVertexArrays(1, &m_oQuad.m_uiVAO);
	//The new buffer need to be binded
	glBindVertexArray(m_oQuad.m_uiVAO);
	//Now we bind the other two buffers 
	glBindBuffer(GL_ARRAY_BUFFER, m_oQuad.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oQuad.m_uiIBO);
	//VBO data buffering
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexNormal), _vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), NULL);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 1));

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 2));

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 3));

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_quadIndex), _quadIndex, GL_STATIC_DRAW);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Let's clean the vertices create a moment ago to avoid memory leaking.
	//delete[] _vertexData;
}

void AdvanceTexture::IncreaseValue()
{
	if (m_bKeys[GLFW_KEY_PAGE_UP])
		m_fSinAug += 0.1f;
	if (m_bKeys[GLFW_KEY_PAGE_DOWN])
		m_fSinAug -= 0.1f;
	if (m_bKeys[GLFW_KEY_HOME])
		m_fTimeAug++;
	if (m_bKeys[GLFW_KEY_END])
		m_fTimeAug--;
	if (m_bKeys[GLFW_KEY_C])
		glEnable(GL_CULL_FACE);
	if (m_bKeys[GLFW_KEY_V])
		glDisable(GL_CULL_FACE);

	if (m_bKeys[GLFW_KEY_P])
		m_fModRGB -= 0.01f;
	if (m_bKeys[GLFW_KEY_O])
		m_fModRGB += 0.01f;
	if (m_fModRGB >= 1.0f)
	{
		m_fModRGB = 1.0f;
	}
	if (m_fModRGB <= 0.0f)
	{
		m_fModRGB = 0.0f;
	}
}

void AdvanceTexture::LoadTexture()
{
	GLint	_width,
			_height,
			_channels;

	unsigned char* _data = stbi_load("./textures/rock_diffuse.tga",
		&_width, 
		&_height, 
		&_channels, 
		STBI_default);

	glGenTextures(1, &m_uiDiffuseTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiDiffuseTexture);

	glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(_data); 
	
	_data = stbi_load("./textures/rock_normal.tga",
		&_width,
		&_height,
		&_channels,
		STBI_default);

	glGenTextures(1, &m_uiNormalTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalTexture);

	glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(_data);

	_data = stbi_load("./textures/rock_specular.tga",
		&_width,
		&_height,
		&_channels,
		STBI_default);

	glGenTextures(1, &m_uiSpecularTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiSpecularTexture);

	glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(_data);
}