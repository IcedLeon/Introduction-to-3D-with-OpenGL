#include "CollisionProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"
#include "tiny_obj_loader.h"

void RenderPlane(vec4 a_vPlane)
{
	vec3 _up = vec3(0, 1, 0);

	if (a_vPlane.xyz() == _up)
	{
		_up = vec3(1, 0, 0);
	}
	//Calculate tangent and bitangent
	vec3 _tangent	= glm::normalize(glm::cross(a_vPlane.xyz(), _up));
	vec3 _bitangent = glm::normalize(glm::cross(a_vPlane.xyz(), _tangent));

	vec3 _point		= a_vPlane.xyz() * a_vPlane.w;

	vec3 _v0		= _point + _tangent + _bitangent;
	vec3 _v1		= _point + _tangent - _bitangent;
	vec3 _v2		= _point + _tangent - _bitangent;
	vec3 _v3		= _point + _tangent + _bitangent;

	Gizmos::addTri(_v0, _v1, _v2, vec4(1, 1, 0, 1));
	Gizmos::addTri(_v0, _v2, _v3, vec4(1, 1, 0, 1));

	Gizmos::addLine(_point, _point + a_vPlane.xyz() * 2, vec4(0, 0, 1, 1));
}

AABB GenerateAABB(vec3* a_vPos, GLuint a_uiCount, GLuint a_uiStride = 0)
{
	AABB _result = {};

	if (a_uiStride == 0)
	{
		a_uiStride = sizeof(vec3);
	}

	_result.m_vMin = a_vPos[0];
	_result.m_vMax = a_vPos[0];

	for (GLuint pos_idx = 0; pos_idx < a_uiCount; ++pos_idx)
	{
		if (a_vPos->x < _result.m_vMin.x) _result.m_vMin.x = a_vPos->x;
		if (a_vPos->y < _result.m_vMin.y) _result.m_vMin.y = a_vPos->y;
		if (a_vPos->z < _result.m_vMin.z) _result.m_vMin.z = a_vPos->z;

		if (a_vPos->x > _result.m_vMax.x) _result.m_vMax.x = a_vPos->x;
		if (a_vPos->y > _result.m_vMax.y) _result.m_vMax.y = a_vPos->y;
		if (a_vPos->z > _result.m_vMax.z) _result.m_vMax.z = a_vPos->z;

		a_vPos = (vec3*)((char*)a_vPos + a_uiStride);
	}

	return _result;
}

void RenderAABB(AABB a_oBoundingBox, mat4 a_mTransform)
{
	vec3 _center	= ((a_oBoundingBox.m_vMin + a_mTransform[3].xyz()) + 
		(a_oBoundingBox.m_vMax + a_mTransform[3].xyz()));
	vec3 _extents = ((a_oBoundingBox.m_vMax + a_mTransform[3].xyz()) -
		(a_oBoundingBox.m_vMin + a_mTransform[3].xyz()));

	Gizmos::addAABB(_center, _extents, vec4(1), &a_mTransform);
}

void GetFrustumPlanes(mat4 a_vProjView, vec4* a_vPlanes)
{
	// right side
	a_vPlanes[0] = vec4(a_vProjView[0][3] - a_vProjView[1][0],
						a_vProjView[1][3] - a_vProjView[1][0],
						a_vProjView[2][3] - a_vProjView[2][0],
						a_vProjView[3][3] - a_vProjView[3][0]);
	// left side
	a_vPlanes[1] = vec4(a_vProjView[0][3] + a_vProjView[0][0],
						a_vProjView[1][3] + a_vProjView[1][0],
						a_vProjView[2][3] + a_vProjView[2][0],
						a_vProjView[3][3] + a_vProjView[3][0]);
	// top
	a_vPlanes[2] = vec4(a_vProjView[0][3] - a_vProjView[0][1],
						a_vProjView[1][3] - a_vProjView[1][1],
						a_vProjView[2][3] - a_vProjView[2][1],
						a_vProjView[3][3] - a_vProjView[3][1]);
	// bottom
	a_vPlanes[3] = vec4(a_vProjView[0][3] + a_vProjView[0][1],
						a_vProjView[1][3] + a_vProjView[1][1],
						a_vProjView[2][3] + a_vProjView[2][1],
						a_vProjView[3][3] + a_vProjView[3][1]);
	// far
	a_vPlanes[4] = vec4(a_vProjView[0][3] - a_vProjView[0][2],
						a_vProjView[1][3] - a_vProjView[1][2],
						a_vProjView[2][3] - a_vProjView[2][2],
						a_vProjView[3][3] - a_vProjView[3][2]);
	// near
	a_vPlanes[5] = vec4(a_vProjView[0][3] + a_vProjView[0][2],
						a_vProjView[1][3] + a_vProjView[1][2],
						a_vProjView[2][3] + a_vProjView[2][2],
						a_vProjView[3][3] + a_vProjView[3][2]);

	for (int i = 0; i < 6; i++)
		a_vPlanes[i] = glm::normalize(a_vPlanes[i]);
}

bool OnPositivePlaneSide(vec4 a_vPlane, AABB a_oBox, mat4 a_vTransform)
{
	vec3 _points[8];

	glm::vec3 _center = (a_oBox.m_vMin + a_oBox.m_vMax) * 0.5f;
	glm::vec3 _extents = (a_oBox.m_vMax - a_oBox.m_vMin) * 0.5f;

	for (unsigned int p = 0; p < 8; ++p)
	{
		_points[p] = _center;
	}

	_points[0] += vec3(_extents.x, _extents.y, _extents.z);
	_points[1] += vec3(_extents.x, _extents.y, _extents.z);
	_points[2] += vec3(_extents.x, _extents.y, _extents.z);
	_points[3] += vec3(_extents.x, _extents.y, _extents.z);

	_points[4] += vec3(_extents.x, _extents.y, _extents.z);
	_points[5] += vec3(_extents.x, _extents.y, _extents.z);
	_points[6] += vec3(_extents.x, _extents.y, _extents.z);
	_points[7] += vec3(_extents.x, _extents.y, _extents.z);

	bool _result = false;

	for (unsigned int p = 0; p < 8; ++p)
	{
		vec4 _transformedPoint = a_vTransform * vec4(_points[p], 1);
		float _dist = glm::dot(a_vPlane.xyz(), _points[p]) + a_vPlane.w;

		if (_dist > 0)
		{
			_result = true;
		}
	}

	return _result;
}

void CollisionP::LoadMesh(const char* a_pccFileName)
{
	vector<tinyobj::shape_t> _shapes;
	vector < tinyobj::material_t> _materials;

	tinyobj::LoadObj(_shapes, _materials, a_pccFileName);

	GLuint _meshCount = m_oMesh.size();

	m_oMesh.resize(m_oMesh.size() + _shapes.size());

	for (GLuint shape_idx = 0; shape_idx < _shapes.size(); ++shape_idx)
	{
		GLuint mesh_idx			= _meshCount + shape_idx;

		std::vector<float> vertex_data;

		unsigned int float_count = _shapes[shape_idx].mesh.positions.size();
		float_count				+= _shapes[shape_idx].mesh.normals.size();

		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
						   _shapes[shape_idx].mesh.positions.begin(),
						   _shapes[shape_idx].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
						   _shapes[shape_idx].mesh.normals.begin(),
						   _shapes[shape_idx].mesh.normals.end());

		m_oMesh[mesh_idx].m_uiIndexCount = _shapes[shape_idx].mesh.indices.size();

		glGenVertexArrays(1, &m_oMesh[mesh_idx].m_uiVAO);
		glGenBuffers(1, &m_oMesh[mesh_idx].m_uiVBO);
		glGenBuffers(1, &m_oMesh[mesh_idx].m_uiIBO);

		glBindVertexArray(m_oMesh[mesh_idx].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_oMesh[mesh_idx].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*float_count, vertex_data.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oMesh[mesh_idx].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			_shapes[shape_idx].mesh.indices.size() * sizeof(GLuint),
			_shapes[shape_idx].mesh.indices.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*_shapes[shape_idx].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_oMesh[mesh_idx].m_oAABB = GenerateAABB(
			(vec3*)&_shapes[shape_idx].mesh.positions[0],
			_shapes[shape_idx].mesh.positions.size() / 3);
	}
}

CollisionP::CollisionP() : m_vAmbientLight(vec3(0)),
						   m_vLightDir(vec3(0)),
						   m_vLightColour(vec3(0)),
						   m_vMaterialColour(vec3(0)),
						   m_fSpecularPower(NULL),
						   m_fTimer(NULL)
{

}

CollisionP::~CollisionP()
{

}

void CollisionP::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oShader.CreateShaderProgram("./shaders/LightingVertex.glsl", "./shaders/LightingFragment.glsl");

	LoadMesh("./models/stanford/dragon.obj");

	float _extents = 100.0f;

	for (GLuint mesh_idx = 0; mesh_idx < 20; ++mesh_idx)
	{
		m_oMesh.push_back(m_oMesh[0]);
		//m_oMesh.back().m_mTransform =
		//	glm::translate(
		//	vec3(glm::linearRand(-_extents, _extents), 
		//		 glm::linearRand(-_extents, _extents),
		//		 glm::linearRand(-_extents, _extents))) *
		//	glm::rotate(
		//		 glm::linearRand(-6.f, 6.f), 
		//		 glm::normalize(
		//		 vec3(glm::linearRand(-10, 10), 
		//			  glm::linearRand(-10, 10), 
		//			  glm::linearRand(-10, 10))));
	}

	m_vAmbientLight = vec3(0.1f);
	m_vLightDir = vec3(0, -1, 0);
	m_vLightColour = vec3(0.6f, 0, 0);
	m_vMaterialColour = vec3(1);
	m_fSpecularPower = 15;
}

void CollisionP::CleanUpWin()
{
	Gizmos::destroy();
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void CollisionP::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	m_fTimer += (float)a_dDeltaTime;

	Gizmos::clear();

	Gizmos::addTransform(mat4(1));

	vec4 _black(0, 0, 0, 1);
	vec4 _blue(0, 0, 1, 1);
	vec4 _red(1, 0, 0, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? _blue : _black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? _red : _black);
	}

	MoveCamera((float)a_dDeltaTime);

	//BoundingSphere _collSphere;
	//vec4 _plane(0, 1, 0, -1);
	//_collSphere.m_vCentre = vec3(0, cosf(m_fTimer) + 1, 0);
	//_collSphere.m_fRadius = 0.5f;
	//Gizmos::addSphere(_collSphere.m_vCentre, _collSphere.m_fRadius, 50, 50, _green);
	//switch (_collSphere.CheckCollision(_plane))
	//{
	//case 1:
	//	_planeColour = _green; //Green
	//	break;
	//case -1:
	//	_planeColour = _yellow; //Yellow
	//	break;
	//case 0:
	//	_planeColour = _red; //Red
	//	break;
	//}
	//Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(4, 1, -4), _planeColour);
	//Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, 4), vec3(-4, 1, -4), _planeColour);
}

void CollisionP::Draw()
{
	vec4 _white(1);
	vec4 _black(0, 0, 0, 1);
	vec4 _red(1, 0, 0, 1);
	vec4 _green(0, 1, 0, 1);
	vec4 _blue(0, 0, 1, 1);
	vec4 _yellow(1, 1, 0, 1);

	Use();

	mat4 _projview = m_oProjection * m_oView;
	Gizmos::draw(_projview);

	vec4 _planes[6];

	GetFrustumPlanes(m_oCamera.GetProjViewTransform(vec2(m_vScreenSize.x, m_vScreenSize.y)), _planes);
	
	vec4 _planeColour = _black;

	for (unsigned int mesh_idx = 0; mesh_idx < m_oMesh.size(); ++mesh_idx)
	{
		bool _isInFrustum = true;

		for (unsigned int plane_idx = 0; plane_idx < 6; ++plane_idx)
		{
			if (!OnPositivePlaneSide(_planes[plane_idx], 
									 m_oMesh[mesh_idx].m_oAABB, 
									 m_oMesh[mesh_idx].m_mTransform) )
			{
				_isInFrustum = false;
				break;
			}
			else
			{
				static int _timeCulled = 0;
				printf("Mesh number: %d has been culled on plane: %d at time: %d \n", mesh_idx, plane_idx, _timeCulled);
			}
		}

		if (_isInFrustum)
		{
			//GLuint _modelUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
			//glUniformMatrix4fv(_modelUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));
			DrawMesh(m_oMesh[mesh_idx]);
		}
	}
}

void CollisionP::MoveCamera(float a_fDeltaTime)
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

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (m_bKeys[GLFW_KEY_C])
		glEnable(GL_CULL_FACE);
	if (m_bKeys[GLFW_KEY_V])
		glDisable(GL_CULL_FACE);
}

void CollisionP::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));
	
	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
	//Camera position
	GLint _eyeUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "eye_pos");
	glUniform3fv(_eyeUni, 1, (float*)&m_oWorld[3].xyz);
	//Ambient light uniform
	GLint _ambientUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "ambient_light");
	glUniform3fv(_ambientUni, 1, (float*)&m_vAmbientLight);
	//Light direction uniform
	GLint _lightDirUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_direction");
	glUniform3fv(_lightDirUni, 1, (float*)&m_vLightDir);
	//Light colour uniform
	GLint _lightColUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "light_colour");
	glUniform3fv(_lightColUni, 1, (float*)&m_vLightColour);
	//Material colour uniform
	GLint _materialUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "material_colour");
	glUniform3fv(_materialUni, 1, (float*)&m_vMaterialColour);
	//Specular power uniform
	GLint _powerUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "specular_power");
	glUniform1f(_powerUni, m_fSpecularPower);
}

void CollisionP::DrawMesh(MeshObj a_oMesh)
{
	glBindVertexArray(a_oMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, a_oMesh.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}

//void CollisionP::GetFrustumPlanes(const mat4& a_mTransform, vec4* a_vPlanes)
//{
//	//right										
//	a_vPlanes[0] = vec4(a_mTransform[0][3] - a_mTransform[0][0],
//						a_mTransform[1][3] - a_mTransform[1][0],
//						a_mTransform[2][3] - a_mTransform[2][0],
//						a_mTransform[3][3] - a_mTransform[3][0]);
//	//left
//	a_vPlanes[1] = vec4(a_mTransform[0][3] + a_mTransform[0][0],
//						a_mTransform[1][3] + a_mTransform[1][0],
//						a_mTransform[2][3] + a_mTransform[2][0],
//						a_mTransform[3][3] + a_mTransform[3][0]);
//	//top
//	a_vPlanes[3] = vec4(a_mTransform[0][3] - a_mTransform[0][1],
//						a_mTransform[1][3] - a_mTransform[1][1],
//						a_mTransform[2][3] - a_mTransform[2][1],
//						a_mTransform[3][3] - a_mTransform[3][1]);
//	//bottom
//	a_vPlanes[3] = vec4(a_mTransform[0][3] + a_mTransform[0][1],
//						a_mTransform[1][3] + a_mTransform[1][1],
//						a_mTransform[2][3] + a_mTransform[2][1],
//						a_mTransform[3][3] + a_mTransform[3][1]);
//	//far
//	a_vPlanes[3] = vec4(a_mTransform[0][3] - a_mTransform[0][2],
//						a_mTransform[1][3] - a_mTransform[1][2],
//						a_mTransform[2][3] - a_mTransform[2][2],
//						a_mTransform[3][3] - a_mTransform[3][2]);
//	//near
//	a_vPlanes[3] = vec4(a_mTransform[0][3] + a_mTransform[0][2],
//						a_mTransform[1][3] + a_mTransform[1][2],
//						a_mTransform[2][3] + a_mTransform[2][2],
//						a_mTransform[3][3] + a_mTransform[3][2]);
//	//normalize them all
//	for (int i = 0; i < 6; i++)
//		a_vPlanes[i] = glm::normalize(a_vPlanes[i]);
//}

//void  CollisionP::DrawFrustum(vec4 a_vPlane)
//{
//	vec3 _tangent = glm::normalize(glm::cross(a_vPlane.xyz(), vec3(0, 1, 0)));
//	vec3 _bitangent = glm::normalize(glm::cross(a_vPlane.xyz(), _tangent));
//
//	vec3 _p = a_vPlane.xyz() * a_vPlane.w;
//
//	vec3 _v0 = _tangent + _bitangent;
//	vec3 _v1 = _tangent - _bitangent;
//	vec3 _v2 = _tangent - _bitangent;
//	vec3 _v3 = _tangent + _bitangent;
//
//	Gizmos::addTri(_v0, _v1, _v2, vec4(1, 1, 0, 1));
//	Gizmos::addTri(_v0, _v2, _v3, vec4(1, 1, 0, 1));
//
//	Gizmos::addLine(_p, _p + a_vPlane.xyz() * 2, vec4(0, 0, 1, 0));
//}

//void  CollisionP::DrawAABB(AABB a_oCollisionBox, mat4 a_mTransform)
//{
//	glm::vec3 _center = ((a_oCollisionBox.m_vMin + a_mTransform[3].xyz()) + (a_oCollisionBox.m_vMax + a_mTransform[3].xyz())) * 0.5f;
//	glm::vec3 _extents = (a_oCollisionBox.m_vMax + a_mTransform[3].xyz()) - (a_oCollisionBox.m_vMin + a_mTransform[3].xyz())) * 0.5f;
//
//	Gizmos::addAABB(_center, _extents, vec4(1, 0, 0, 1));
//}
