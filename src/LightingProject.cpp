#include "LightingProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

Lighting::Lighting() :	m_vLightColour(vec3(0.0f)),
						m_fLighColourX(NULL),
						m_fLighColourY(NULL),
						m_fLighColourZ(NULL),
						m_vLightDir(vec3(0.0f)),
						m_fDirX(NULL),
						m_fDirY(NULL),
						m_fDirZ(NULL),
						m_vAmbientLight(vec3(0.0f)),
						m_fAmbientMod(NULL),
						m_vMaterialColour(vec3(0.0f)),
						m_fMaterialX(NULL),
						m_fMaterialY(NULL),
						m_fMaterialZ(NULL),
						m_fSpecularPwr(NULL),
						m_fDirDT(NULL),
						m_fColDT(NULL),
						m_bRandLight(false),
						m_bRandLightColour(false),
						m_bRandMaterial(false)
{

}

Lighting::~Lighting()
{

}

void Lighting::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	m_vAmbientLight		= vec3(0.1f);
	m_vLightColour		= vec3(1, 1, 1);
	m_vLightDir			= vec3(0, -1, 0);
	m_vMaterialColour	= vec3(1, 1, 0);
	m_fSpecularPwr		= 15.0f;

	m_oShader.CreateShaderProgram(LIGHTING_VERTEX_GLSL, LIGHTING_FRAGMENT_GLSL);

	LoadModel("./models/stanford/dragon.obj");

}

void Lighting::LoadModel(const char* a_pccFileName)
{
	std::string err = tinyobj::LoadObj(m_oShapes, m_oMaterials, a_pccFileName);
	if (err.size() != NULL)
	{
		printf("ERROR: <MODEL COULD NOT BE LOADED CORRECTLY>. \n");
	}
	CreateOpenGLBuffers(m_oShapes);
}

void Lighting::CleanUpWin()
{
	CleanupOpenGLBuffers();
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void Lighting::Update(GLdouble a_fDeltaTime)
{
	m_oView		= m_oCamera.GetViewTransform();
	m_oWorld	= m_oCamera.GetWorldTransform();

	MoveCamera((float)a_fDeltaTime);
	OpenGLOptionSwitch();
	UpdateLightingInput();
}

void Lighting::Draw()
{
	Use();
	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_uiVAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_uiIndexCount, GL_UNSIGNED_INT, 0);
	}
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
}

void Lighting::Use()
{
	m_oShader.Use();
	GrabUniformLoc();
}

void Lighting::GrabUniformLoc()
{
	//Projection uniform.
	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));
	//View uniform
	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
	//Model/world uniform
	//mat4 _model = glm::translate(_model, )
	//GLuint _worldUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
	//glUniformMatrix4fv(_worldUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));
	//Ambient light uniform
	GLuint _ambientUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "ambient_light");
	glUniform3fv(_ambientUni, 1, (float*)&m_vAmbientLight);
	//Light direction uniform
	GLuint _lightDirUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_direction");
	glUniform3fv(_lightDirUni, 1, (float*)&m_vLightDir);
	//Light colour uniform
	GLuint _lightColUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_colour");
	glUniform3fv(_lightColUni, 1, (float*)&m_vLightColour);
	//Material colour uniform
	GLuint _materialUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "material_colour");
	glUniform3fv(_materialUni, 1, (float*)&m_vMaterialColour);
	//Camera position
	GLuint _eyeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "eye_pos");
	glUniform3fv(_eyeUni, 1, (float*)&m_oCamera.GetWorldTransform()[3].xyz);
	//Specular power uniform
	GLuint _powerUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_power");
	glUniform1f(_powerUni, m_fSpecularPwr);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*float_count, vertex_data.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[shape_indx].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			shapes[shape_indx].mesh.indices.size() * sizeof(GLuint), 
			shapes[shape_indx].mesh.indices.data(), GL_DYNAMIC_DRAW);

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

void Lighting::ReloadShader()
{
	glDeleteProgram(m_oShader.GetShaderProgram());
	m_oShader.CreateShaderProgram(LIGHTING_VERTEX_GLSL, LIGHTING_FRAGMENT_GLSL);
}

void Lighting::OpenGLOptionSwitch()
{
	if (m_bKeys[GLFW_KEY_TAB])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (m_bKeys[GLFW_KEY_C])
	{
		glEnable(GL_CULL_FACE);
	}
	if (m_bKeys[GLFW_KEY_V])
	{
		glDisable(GL_CULL_FACE);
	}
	if (m_bKeys[GLFW_KEY_1])
	{
		LoadModel("./models/stanford/dragon.obj");
	}
	if (m_bKeys[GLFW_KEY_2])
	{
		LoadModel("./models/stanford/buddha.obj");
	}
	if (m_bKeys[GLFW_KEY_3])
	{
		LoadModel("./models/stanford/bunny.obj");
	}
}

void Lighting::UpdateLightingInput()
{
	m_fDirDT += (float)GetDelta();
	m_fColDT += (float)GetDelta() * 0.25f;
	//Reload shaders
	if (m_bKeys[GLFW_KEY_R])
	{
		ReloadShader();
		printf("SHADER RELOADED SUCCESFULLY.");
	}
	//Increase the value of the specular power.
	if (m_bKeys[GLFW_KEY_PAGE_UP])
	{
		m_fSpecularPwr += 0.1f;
		printf("SPECULAR POWER: %f\n", m_fSpecularPwr);
	}
	//Decrease the value of the specular power.
	if (m_bKeys[GLFW_KEY_PAGE_DOWN])
	{
		//Check if the specular power do not drop under 0;
		if (m_fSpecularPwr <= 0.0f)
		{
			m_fSpecularPwr = 0.0f;
		}
		else
		{
			m_fSpecularPwr -= 0.1f;
		}
		printf("SPECULAR POWER: %f\n", m_fSpecularPwr);
	}
	//Light direction X
	if (m_bKeys[GLFW_KEY_N])
	{
		if (m_fDirX <= 1.0f)
		{
			m_fDirX += 0.1f;
		}
		else
		{
			m_fDirX = 1.0f;
		}
		printf("LIGHT DIRECTION X: %f\n", m_fDirX);
		m_vLightDir.x = m_fDirX;
	}
	if (m_bKeys[GLFW_KEY_M])
	{
		if (m_fDirX >= -1.0f)
		{
			m_fDirX -= 0.1f;
		}
		else
		{
			m_fDirX = -1.0f;
		}
		printf("LIGHT DIRECTION X: %f\n", m_fDirX);
		m_vLightDir.x = m_fDirX;
	}
	//Ligh direction Y
	if (m_bKeys[GLFW_KEY_K])
	{
		if (m_fDirY <= 1.0f)
		{
			m_fDirY += 0.1f;
		}
		else
		{
			m_fDirY = 1.0f;
		}
		printf("LIGHT DIRECTION Y: %f\n", m_fDirY);
		m_vLightDir.y = m_fDirY;
	}
	if (m_bKeys[GLFW_KEY_L])
	{
		if (m_fDirY >= -1.0f)
		{
			m_fDirY -= 0.1f;
		}
		else
		{
			m_fDirY = -1.0f;
		}
		printf("LIGHT DIRECTION Y: %f\n", m_fDirY);
		m_vLightDir.y = m_fDirY;
	}
	//Ligh direction Z
	if (m_bKeys[GLFW_KEY_O])
	{
		if (m_fDirZ <= 1.0f)
		{
			m_fDirZ += 0.1f;
		}
		else
		{
			m_fDirZ = 1.0f;
		}
		printf("LIGHT DIRECTION Z: %f\n", m_fDirZ);
		m_vLightDir.z = m_fDirZ;
	}
	if (m_bKeys[GLFW_KEY_P])
	{
		if (m_fDirZ >= -1.0f)
		{
			m_fDirZ -= 0.1f;
		}
		else
		{
			m_fDirZ = -1.0f;
		}
		printf("LIGHT DIRECTION Z: %f\n", m_fDirZ);
		m_vLightDir.z = m_fDirZ;
	}
	//Rand light dir
	if (m_bKeys[GLFW_KEY_G])
	{
		m_bRandLight = true;
		printf("RAND LIGHT ON.");
	}
	if (m_bKeys[GLFW_KEY_H])
	{
		m_bRandLight = false;
		printf("RAND LIGHT OFF.");
	}
	//Rand colour
	if (m_bKeys[GLFW_KEY_T])
	{
		m_bRandLightColour = true;
		printf("RAND LIGHT COLOUR ON.");
	}
	if (m_bKeys[GLFW_KEY_Y])
	{
		m_bRandLightColour = false;
		printf("RAND LIGHT COLOUR OFF.");
	}
	//Rand material
	if (m_bKeys[GLFW_KEY_U])
	{
		m_bRandMaterial = true;
		printf("RAND MATERIAL COLOUR ON.");
	}
	if (m_bKeys[GLFW_KEY_I])
	{
		m_bRandMaterial = false;
		printf("RAND MATERIAL COLOUR OFF.");
	}
	//
	if (m_bRandLight)
	{
		m_fDirX = sinf(m_fDirDT);
		m_fDirY = 1.0f;
		m_fDirZ = cosf(m_fDirDT);
		m_vLightDir = vec3(m_fDirX, m_fDirY, m_fDirZ);
	}
	//
	if (m_bRandLightColour)
	{
		m_fLighColourX = cosf(m_fColDT);
		m_fLighColourY = sinf(m_fColDT);
		m_fLighColourZ = cosf(m_fColDT);
		m_vLightColour = vec3(m_fLighColourX, m_fLighColourY, m_fLighColourZ);
	}
	if (m_bRandMaterial)
	{
		m_fMaterialX = sinf(m_fColDT);
		m_fMaterialY = cosf(m_fColDT);
		m_fMaterialZ = sinf(m_fColDT);
		m_vMaterialColour = vec3(m_fMaterialX, m_fMaterialY, m_fMaterialZ);
	}
}