#include "GeometryProject.h"
#include <GLFW\glfw3.h>
#include "Vertex.h"
#include "Camera.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

GeometryProject::GeometryProject() : m_fTimer(NULL),
									 m_fSinAug(NULL),
									 m_fTimeAug(NULL),
									 m_uiVAO(NULL),
									 m_uiVBO(NULL),
									 m_uiIBO(NULL),
									 m_uiIndexCount(NULL),
									 m_fModRGB(1)
{

}

GeometryProject::~GeometryProject()
{

}

void GeometryProject::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	m_oShader.CreateShaderProgram(GEOMETRY_VERTEX_GLSL, GEOMETRY_FRAGMENT_GLSL);

	LoadTexture("./textures/crate.png");

	//GenerateGrid(10, 25);

	//GenerateCube();

	GenerateQuad(5.0f);
}

void GeometryProject::CleanUpWin()
{
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void GeometryProject::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	MoveCamera((float)a_fDeltaTime);

	IncreaseValue();

	m_fTimer += (float)GetDelta() * m_fTimeAug;
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

void GeometryProject::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	GLint _timeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "time");
	glUniform1f(_timeUni, m_fTimer);

	GLuint _perturbanceHeight = glGetUniformLocation(m_oShader.GetShaderProgram(), "heightScale");
	glUniform1f(_perturbanceHeight, m_fSinAug);

	vec3 _p = vec3(152.0f, 152.0f, 0.0f);

	vec3 _i = floor(_p);
	vec3 _f = glm::fract(_p);
	vec3 _u = _f * _f * (3.0f - 2.0f * _f);
	float _resultNoise = -1.0f + 2.0f * glm::mix(glm::mix(Hash(_i + vec3(0, 0, 0)), Hash(_i + vec3(1.0f, 0.0f, 0.0f)), _u.x),
		glm::mix(Hash(_i + vec3(0.0f, 1.0f, 0.0f)), Hash(_i + vec3(1.0f, 1.0f, 0.0f)), _u.x), _u.y);
	GLuint _noise = glGetUniformLocation(m_oShader.GetShaderProgram(), "noise");
	glUniform1f(_noise, _resultNoise);

	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);

	GLuint _textUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "diffuse");
	glUniform1i(_textUni, 0);

	GLuint _modRGBUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "modColor");
	glUniform1f(_modRGBUni, m_fModRGB);

	glBindVertexArray(m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	//glDrawElements(GL_TRIANGLES, m_uiIndexCount, GL_UNSIGNED_INT, NULL);
	//glBindVertexArray(m_uiVAOc);
	//glDrawElements(GL_TRIANGLES, m_uiCubeIndex * 4, GL_UNSIGNED_INT, NULL);

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

void GeometryProject::GenerateQuad(float a_fSize)
{
	Vertex _vertexData[4];
	int _index = 0;

	_vertexData[_index++].m_vPos = vec4(-a_fSize, 0.0f, -a_fSize, 1);
	_vertexData[_index++].m_vPos = vec4(-a_fSize, 0.0f, a_fSize, 1);
	_vertexData[_index++].m_vPos = vec4(a_fSize, 0.0f, a_fSize, 1);
	_vertexData[_index++].m_vPos = vec4(a_fSize, 0.0f, -a_fSize, 1);
	_index = 0;
	_vertexData[_index++].m_vUV = vec2(0, 0);
	_vertexData[_index++].m_vUV = vec2(0, 1);
	_vertexData[_index++].m_vUV = vec2(1, 1);
	_vertexData[_index++].m_vUV = vec2(1, 0);

	GLuint _quadIndex[6] = { 0, 1, 2, 0, 2, 3 };

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
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), _vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)*2));
	//IBO data buffering
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_quadIndex), _quadIndex, GL_STATIC_DRAW);
	//And we wrap de-binding the buffers.
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Let's clean the vertices create a moment ago to avoid memory leaking.
	//delete[] _vertexData;
}

void GeometryProject::GenerateCube()
{
	m_uiCubeIndex = 10;
	Vertex* _cubeVertex = new Vertex[m_uiCubeIndex];

	for (unsigned int i = 0; i < m_uiCubeIndex; ++i)
	{
		switch (i)
		{
			//Front
		case 0:
			_cubeVertex[i].m_vPos = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV  = vec2(0.0f, 0.0f);
			break;
		case 1:
			_cubeVertex[i].m_vPos = vec4(1.0f, -1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(1.0f, 0.0f);
			break;
		case 2:
			_cubeVertex[i].m_vPos = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(1.0f, 1.0f);
			break;
		case 3:
			_cubeVertex[i].m_vPos = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(0.0f, 1.0f);
			break;
			//Back
		case 4:
			_cubeVertex[i].m_vPos = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(0.0f, 1.0f);
			break;
		case 5:
			_cubeVertex[i].m_vPos = vec4(1.0f, -1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(1.0f, 0.0f);
			break;
		case 6:
			_cubeVertex[i].m_vPos = vec4(1.0f, 1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(1.0f, 1.0f);
			break;
		case 7:
			_cubeVertex[i].m_vPos = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
			_cubeVertex[i].m_vCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_cubeVertex[i].m_vUV = vec2(0.0f, 1.0f);
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
	glGenBuffers(1, &m_uiVBOc);
	glGenBuffers(1, &m_uiIBOc);
	//Now it's time to generate the vertex array buffer
	glGenVertexArrays(1, &m_uiVAOc);
	//The new buffer need to be binded
	glBindVertexArray(m_uiVAOc);
	//Now we bind the other two buffers 
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBOc);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBOc);
	//VBO data buffering
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), _cubeVertex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)* 2));
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

float GeometryProject::Hash(vec3 _p)
{
	float _h = glm::dot(_p, vec3(127.1, 311.7, 0.0f));

	return glm::fract(sin(_h) * 43758.5453123f);
}

void GeometryProject::LoadTexture(const char* a_pccFileName)
{
	GLint _width,
		_height,
		_channels;

	unsigned char* _data = stbi_load(a_pccFileName, &_width, &_height, &_channels, STBI_default);

	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);

	glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(_data);
}