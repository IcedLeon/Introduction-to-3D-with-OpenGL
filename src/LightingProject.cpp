#include "LightingProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "FBXFile.h"

static vec4 m_vWhite = vec4(1.0f);
static vec4 m_vBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vRed = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 m_vGreen = vec4(0.0f, 1.0f, 0.0f, 1.0f);
static vec4 m_vBlue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

Camera Lighting::m_oCamera = Camera();
bool Lighting::m_bKeys[1024];
static bool m_bMouse;
static GLdouble m_fPrevX;
static GLdouble m_fPrevY;

Lighting::Lighting() : m_fTimer(NULL),
					   m_fSinAug(NULL),
					   m_fTimeAug(NULL)
{

}

Lighting::~Lighting()
{

}

void Lighting::InitWindow(vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vScreenSize, a_pccWinName, a_bFullScreen);

	glfwSetKeyCallback(m_oWin, key_callback);
	glfwSetCursorPosCallback(m_oWin, mouse_callback);
	glfwSetCursorPos(m_oWin, (double)m_vScreenSize.x / 2, (double)m_vScreenSize.y / 2);
	glfwSetScrollCallback(m_oWin, scroll_callback);

	m_oCamera.BuildCamera(vec3(0.0f, 20.0f, 3.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	m_oShader.CreateShaderProgram(VERTEX_FILE_ID1, FRAGMENT_FILE_ID1);

	m_fbxFileID = new FBXFile();
	m_fbxFileID->load("./models/stanford/Buddha.fbx");

	CreateOpenGLBuffers(m_fbxFileID);
	
	GenerateGrid(20, 20);
}

void Lighting::CleanUpWin()
{
	CleanupOpenGLBuffers();
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void Lighting::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	MoveCamera((float)a_fDeltaTime);

	IncreaseValue();

	m_fTimer += (float)GetDelta() * m_fTimeAug;

	m_fTimer = 0;
}

void Lighting::Draw()
{
	Use();
}

void Lighting::key_callback(GLFWwindow* a_oWindow, int a_iKey, int a_iKeyCode, int a_iAction, int a_iMode)
{
	printf("Pressed Key is: %c\n", a_iKey);
	if (a_iKey == GLFW_KEY_ESCAPE && a_iAction == GLFW_PRESS)
		glfwSetWindowShouldClose(a_oWindow, GL_TRUE);

	if (a_iAction == GLFW_PRESS)
		m_bKeys[a_iKey] = true;
	else if (a_iAction == GLFW_RELEASE)
		m_bKeys[a_iKey] = false;
}

void Lighting::MoveCamera(float a_fDeltaTime)
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
	if (m_bKeys[GLFW_KEY_R])
		SwitchShader();
}

void Lighting::SwitchShader()
{
	glDeleteProgram(m_oShader.GetShaderProgram());
	m_oShader.CreateShaderProgram(VERTEX_FILE_ID1, FRAGMENT_FILE_ID1);
}

void Lighting::mouse_callback(GLFWwindow* a_oWindow, double a_iMouseX, double a_iMouseY)
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

void Lighting::scroll_callback(GLFWwindow* a_oWindow, double a_fOffsetX, double a_fOffsetY)
{
	m_oCamera.MouseScrollZoom((float)a_fOffsetY);
}

void Lighting::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));

	glBindVertexArray(m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_uiIndexCount, GL_UNSIGNED_INT, NULL);

	for (unsigned int mesh_indx = 0; mesh_indx < m_fbxFileID->getMeshCount(); ++mesh_indx)
	{
		FBXMeshNode* _mesh = m_fbxFileID->getMeshByIndex(mesh_indx);

		GLuint* _glData = (GLuint*)_mesh->m_userData;

		glBindVertexArray(_glData[0]);
		glDrawElements(GL_TRIANGLES, (GLuint)_mesh->m_indices.size(), GL_UNSIGNED_INT, NULL);
	}

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Lighting::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
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

void Lighting::IncreaseValue()
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
}

void Lighting::LoadTexture(const char* a_pccFileName)
{
	//GLint _width,
	//	_height,
	//	_channels;

	//unsigned char* _data = stbi_load(a_pccFileName, &_width, &_height, &_channels, STBI_default);

	//glGenTextures(1, &m_uiTexture);
	//glBindTexture(GL_TEXTURE_2D, m_uiTexture);

	//glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGB, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, _data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//stbi_image_free(_data);
}

void Lighting::CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_data.resize(shapes.size());

	for (unsigned int shape_indx = 0; shape_indx < shapes.size(); ++shape_indx)
	{
		std::vector<float> vertex_data;

		unsigned int float_count = shapes[shape_indx].mesh.position.size();

		float_count += shapes[shape_indx].mesh.normals.size();

		vertex_data.reserve(float_count); 

		vertex_data.insert(vertex_data.end(), 
			shapes[shape_indx].mesh.position.begin(),
			shapes[shape_indx].mesh.position.end());

		vertex_data.insert(vertex_data.end(), 
			shapes[shape_indx].mesh.normals.begin(),
			shapes[shape_indx].mesh.normals.end());
		m_gl_data[shape_indx].m_indices_count = shapes[shape_indx].mesh.indices.size();

		glGenVertexArrays(3, GL_FLOAT, 0, 0);
		glGenBuffers(1, &m_gl_data[shape_indx].m_vbo);
		glGenBuffers(1, &m_gl_data[shape_indx].m_IBO);

		glBindVertexArray(m_gl_data[shape_indx].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_data[shape_indx].m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*float_count, vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_data[shape_indx].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			shapes[shape_indx].mesh.indices.size() * sizeof(GLuint), 
			shapes[shape_indx].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::CleanupOpenGLBuffers()
{
	for (unsigned int mesh_indx = 0; mesh_indx < m_gl_data.size(); ++mesh_indx)
	{
		glDeleteVertexArrays(1, &m_gl_data[mesh_indx].m_VAO);
		glDeleteBuffers(2, &m_gl_data[mesh_indx].m_vbo);
		glDeleteBuffers(2, &m_gl_data[mesh_indx].m_IBO);
	}
}