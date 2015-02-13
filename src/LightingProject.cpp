#include "LightingProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

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

	m_oCamera.BuildCamera(vec3(5.0f, 30.0f, 10.0f));

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	ambient_light = vec3(0.4f);
	light_colour = vec3(0.4f, 0.3f, 0.6f);
	light_dir = vec3(0.4, 0.3, 0);
	material_colour = vec3(0.4, 1, 0);
	specular_pwr = 15.0f;

	m_oShader.CreateShaderProgram(VERTEX_FILE_ID1, FRAGMENT_FILE_ID1);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials,
		"./models/stanford/buddha.obj");

	if (err.size() != NULL)
	{
		printf("ERROR: <MODEL NOT LOADED CORRECTLY. \n");
	}

	CreateOpenGLBuffers(shapes);
	
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

	GLuint _ambientUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "ambient_light");
	glUniform3fv(_ambientUni, 1, (float*)&ambient_light);

	GLuint _materialUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "material_colour");
	glUniform3fv(_materialUni, 1, (float*)&material_colour);

	GLuint _lightDirUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_direction");
	glUniform3fv(_lightDirUni, 1, (float*)&light_dir);

	GLuint _lightColUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_colour");
	glUniform3fv(_lightColUni, 1, (float*)&light_colour);

	GLuint _eyeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "eye_pos");
	glUniform3fv(_eyeUni, 1, (float*)&m_oCamera.GetWorldTransform()[3].xyz);

	GLuint _powerUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_power");
	glUniform1f(_powerUni, specular_pwr);

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_uiVAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_uiIndexCount, GL_UNSIGNED_INT, 0);
	}

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Lighting::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
{
	////For better readbility
	//unsigned int _count = a_uiRows * a_uiCols;
	//unsigned int* _auiIndices = new unsigned int[(a_uiRows - 1) * (a_uiCols - 1) * 6];
	//m_uiIndexCount = (a_uiRows - 1) * (a_uiCols - 1) * 6;
	////Create new vertex to allocate position and colours (ATM just that, later we can do more stuff, UV, normal ecc).
	//Vertex* _vertices = new Vertex[(a_uiRows + 1)*(a_uiCols + 1)];
	////Gridding the new vertex to position.
	//for (unsigned int rw = 0; rw < a_uiRows + 1; ++rw)
	//{
	//	for (unsigned int cl = 0; cl < a_uiCols + 1; ++cl)
	//	{
	//		_vertices[rw * a_uiCols + cl].m_vPos = vec4((float)cl, 0.0f, (float)rw, 1);

	//		vec3 _colour = vec3(cosf((cl / (float)(a_uiCols - 1)) * (rw / (float)(a_uiRows - 1))));
	//		_vertices[rw * a_uiCols + cl].m_vCol = vec4(_colour, 1.0f);
	//	}
	//}
	////Assign the indices to determine where to draw our triangles on screen.
	//unsigned int _index = 0;
	//for (unsigned int rw = 0; rw < (a_uiRows - 1); ++rw)
	//{
	//	for (unsigned int cl = 0; cl < (a_uiCols - 1); ++cl)
	//	{
	//		// triangle 1
	//		_auiIndices[_index++] = rw * a_uiCols + cl;
	//		_auiIndices[_index++] = (rw + 1) * a_uiCols + cl;
	//		_auiIndices[_index++] = (rw + 1) * a_uiCols + (cl + 1);
	//		// triangle 2
	//		_auiIndices[_index++] = rw * a_uiCols + cl;
	//		_auiIndices[_index++] = (rw + 1) * a_uiCols + (cl + 1);
	//		_auiIndices[_index++] = rw * a_uiCols + (cl + 1);
	//	}
	//}
	////Generate both the vertex buffer and the index buffer object
	//glGenBuffers(1, &m_uiVBO);
	//glGenBuffers(1, &m_uiIBO);
	////Now it's time to generate the vertex array buffer
	//glGenVertexArrays(1, &m_uiVAO);
	////The new buffer need to be binded
	//glBindVertexArray(m_uiVAO);
	////Now we bind the other two buffers 
	//glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	////VBO data buffering
	//glBufferData(GL_ARRAY_BUFFER, _count * sizeof(Vertex), _vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	////IBO data buffering
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, (a_uiRows - 1) * (a_uiCols - 1) * 6 * sizeof(unsigned int), _auiIndices, GL_DYNAMIC_DRAW);
	////And we wrap de-binding the buffers.
	//glBindVertexArray(NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	////Let's clean the vertices create a moment ago to avoid memory leaking.
	//delete[] _vertices;
	//delete[] _auiIndices;
}

void Lighting::IncreaseValue()
{
	if (m_bKeys[GLFW_KEY_PAGE_UP])
		specular_pwr += 0.1f;
	if (m_bKeys[GLFW_KEY_PAGE_DOWN])
		specular_pwr -= 0.1f;
	if (m_bKeys[GLFW_KEY_HOME])
		m_fTimeAug++;
	if (m_bKeys[GLFW_KEY_END])
		m_fTimeAug--;
	if (m_bKeys[GLFW_KEY_N])

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
	m_gl_info.resize(shapes.size());

	for (unsigned int shape_indx = 0; shape_indx < shapes.size(); ++shape_indx)
	{
		std::vector<float> vertex_data;

		unsigned int float_count = shapes[shape_indx].mesh.positions.size();

		float_count += shapes[shape_indx].mesh.normals.size();

		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(), 
			shapes[shape_indx].mesh.positions.begin(),
			shapes[shape_indx].mesh.positions.end());

		vertex_data.insert(vertex_data.end(), 
			shapes[shape_indx].mesh.normals.begin(),
			shapes[shape_indx].mesh.normals.end());

		m_gl_info[shape_indx].m_uiIndexCount = shapes[shape_indx].mesh.indices.size();

		glGenVertexArrays(1, &m_gl_info[shape_indx].m_uiVAO);
		glGenBuffers(1, &m_gl_info[shape_indx].m_uiVBO);
		glGenBuffers(1, &m_gl_info[shape_indx].m_uiIBO);

		glBindVertexArray(m_gl_info[shape_indx].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[shape_indx].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*float_count, vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[shape_indx].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			shapes[shape_indx].mesh.indices.size() * sizeof(GLuint), 
			shapes[shape_indx].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[shape_indx].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::CleanupOpenGLBuffers()
{
	for (unsigned int mesh_indx = 0; mesh_indx < m_gl_info.size(); ++mesh_indx)
	{
		glDeleteVertexArrays(1, &m_gl_info[mesh_indx].m_uiVAO);
		glDeleteBuffers(2, &m_gl_info[mesh_indx].m_uiVBO);
		glDeleteBuffers(2, &m_gl_info[mesh_indx].m_uiIBO);
	}
}