//#include "CollisionProject.h"
//#include <GLFW\glfw3.h>
//#include "Camera.h"
//#include "Gizmos.h"
//#include "tiny_obj_loader.h"
//#include "BoundingBoxes.h"
//
//CollisionP::CollisionP()
//{
//	m_fTimer = NULL;
//}
//CollisionP::~CollisionP()
//{
//
//}
//
//void CollisionP::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
//{
//	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
//
//	m_oProjection = m_oCamera.GetProjectionTransform(glm::vec2(a_vScreenSize.x, a_vScreenSize.y));
//
//	Gizmos::create();
//
//	m_oShader.CreateShaderProgram("./shaders/LightingVertex.glsl", "./shaders/LightingFragment.glsl");
//}
//
//void CollisionP::CleanUpWin()
//{
//	Gizmos::destroy();
//	m_oShader.CleanUpProgram();
//	Application::CleanUpWin();
//}
//
//void CollisionP::Update(GLdouble a_dDeltaTime)
//{
//	vec4 _white(1);
//	vec4 _black(0, 0, 0, 1);
//	vec4 _red(1, 0, 0, 1);
//	vec4 _green(0, 1, 0, 1);
//	vec4 _blue(0, 0, 1, 1);
//	vec4 _yellow(1, 1, 0, 1);
//
//	m_oView = m_oCamera.GetViewTransform();
//	m_oWorld = m_oCamera.GetWorldTransform();
//
//	m_fTimer += (float)a_dDeltaTime;
//
//	Gizmos::clear();
//
//	Gizmos::addTransform(mat4(1));
//
//	BoundingSphere _collSphere;
//
//	_collSphere.m_vCentre = vec3(0, cosf(m_fTimer) + 1, 0);
//	_collSphere.m_fRadius = 0.5f;
//
//	vec4 _planes[6];
//
//	GetFrustumPlanes(m_oCamera.GetProjViewTransform(vec2(m_vScreenSize.x, m_vScreenSize.y)), _planes);
//
//	vec4 _plane(0, 1, 0, -1);
//
//	vec4 _planeColour = _black;
//
//	Gizmos::addSphere(_collSphere.m_vCentre, _collSphere.m_fRadius, 50, 50, _green);
//
//	switch (_collSphere.CheckCollision(_plane))
//	{
//	case 1:
//		_planeColour = _green; //Green
//		break;
//	case -1:
//		_planeColour = _yellow; //Yellow
//		break;
//	case 0:
//		_planeColour = _red; //Red
//		break;
//	}
//	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(4, 1, -4), _planeColour);
//	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, 4), vec3(-4, 1, -4), _planeColour);
//
//
//	
//	for (unsigned int mesh_idx = 0; mesh_idx < m_oMesh.size(); ++mesh_idx)
//	{
//		bool _isInFrustum = true;
//
//		vec4 _transformPlanes[6];
//		for (unsigned int plane_idx = 0; plane_idx < 6; ++plane_idx)
//		{
//			mat4 _invTransform = glm::inverse(m_oMesh[mesh_idx].m_mTransform);
//
//			vec4 _N = _planes[0];
//			_N.w = 0;
//			vec4 _p = _planes[0] * _planes[0].w;
//			_p.w = 1;
//
//			_N *= _invTransform;
//			_p *= _invTransform;
//
//			_transformPlanes[plane_idx] = _N;
//			_transformPlanes[plane_idx].w = glm::dot(_N.xyz(), _p.xyz());
//		}
//		for (unsigned int plane_idx = 0; plane_idx < 6; ++plane_idx)
//		{
//			if (!OnPositivePlaneSide(_transformPlanes[plane_idx], m_oMesh[mesh_idx].m_oAABB))
//			{
//				_isInFrustum = false;
//				break;
//			}
//			else
//			{
//				static int _timeCulled = 0;
//				printf("")
//			}
//		}
//	}
//
//	for (int i = 0; i <= 20; ++i)
//	{
//		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
//			i == 10 ? _blue : _black);
//		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
//			i == 10 ? _red : _black);
//	}
//
//	MoveCamera((float)a_dDeltaTime);
//}
//
//void CollisionP::Draw()
//{
//	Use();
//	mat4 _projview = m_oProjection * m_oView;
//	Gizmos::draw(_projview);
//}
//
//void CollisionP::MoveCamera(float a_fDeltaTime)
//{
//	// Camera controls
//	if (m_bKeys[GLFW_KEY_W])
//		m_oCamera.KeyboardInput(FORWARD, a_fDeltaTime);
//	if (m_bKeys[GLFW_KEY_S])
//		m_oCamera.KeyboardInput(BACKWARD, a_fDeltaTime);
//	if (m_bKeys[GLFW_KEY_A])
//		m_oCamera.KeyboardInput(LEFT, a_fDeltaTime);
//	if (m_bKeys[GLFW_KEY_D])
//		m_oCamera.KeyboardInput(RIGHT, a_fDeltaTime);
//
//	if (m_bKeys[GLFW_KEY_TAB])
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	if (m_bKeys[GLFW_KEY_C])
//		glEnable(GL_CULL_FACE);
//	if (m_bKeys[GLFW_KEY_V])
//		glDisable(GL_CULL_FACE);
//}
//
//void CollisionP::Use()
//{
//	m_oShader.Use();
//
//	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
//	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));
//	
//	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
//	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
//}
//
//
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
//
//MeshObj CollisionP::LoadMesh(const char* a_pccFileName)
//{
//	vector<tinyobj::shape_t> _shapes;
//	vector < tinyobj::material_t> _materials;
//
//	tinyobj::LoadObj(_shapes, _materials, a_pccFileName);
//
//
//
//	for (GLuint shape_idx = 0; shape_idx < _shapes.size(); ++shape_idx)
//	{
//		std::vector<float> vertex_data;
//
//		unsigned int float_count = _shapes[shape_idx].mesh.positions.size();
//
//		float_count += _shapes[shape_idx].mesh.normals.size();
//
//		vertex_data.reserve(float_count);
//
//		vertex_data.insert(vertex_data.end(),
//			_shapes[shape_idx].mesh.positions.begin(),
//			_shapes[shape_idx].mesh.positions.end());
//
//		vertex_data.insert(vertex_data.end(),
//			_shapes[shape_idx].mesh.normals.begin(),
//			_shapes[shape_idx].mesh.normals.end());
//
//		m_oMesh[shape_idx].m_uiIndexCount = _shapes[shape_idx].mesh.indices.size();
//
//		glGenVertexArrays(1, &m_oMesh[shape_idx].m_uiVAO);
//		glGenBuffers(1, &m_oMesh[shape_idx].m_uiVBO);
//		glGenBuffers(1, &m_oMesh[shape_idx].m_uiIBO);
//
//		glBindVertexArray(m_oMesh[shape_idx].m_uiVAO);
//
//		glBindBuffer(GL_ARRAY_BUFFER, m_oMesh[shape_idx].m_uiVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*float_count, vertex_data.data(), GL_DYNAMIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oMesh[shape_idx].m_uiIBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//			_shapes[shape_idx].mesh.indices.size() * sizeof(GLuint),
//			_shapes[shape_idx].mesh.indices.data(), GL_DYNAMIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
//			(void*)(sizeof(float)*_shapes[shape_idx].mesh.positions.size()));
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		m_oMesh[mesh_index],
//	}
//}
//
//void CollisionP::DrawMesh()
//{
//
//}
//
//bool OnPositivePlaneSide(vec4 a_vPlane, AABB a_oBox, mat4 a_vTransform)
//{
//	vec3 _points[8];
//
//	glm::vec3 _center = (a_oBox.m_vMin + a_oBox.m_vMax) * 0.5f;
//	glm::vec3 _extents = (a_oBox.m_vMax - a_oBox.m_vMin) * 0.5f;
//
//	for (unsigned int p = 0; p < 8; ++p)
//	{
//		_points[p] = _center;
//	}
//	
//	_points[0] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[1] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[2] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[3] += vec3(_extents.x, _extents.y, _extents.z);
//
//	_points[4] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[5] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[6] += vec3(_extents.x, _extents.y, _extents.z);
//	_points[7] += vec3(_extents.x, _extents.y, _extents.z);
//
//	for (unsigned int p = 0; p < 8; ++p)
//	{
//		
//		float _dist = glm::dot(a_vPlane.xyz(), _points[p]) + a_vPlane.w;
//	}
//}
//
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
//
//void  CollisionP::DrawAABB(AABB a_oCollisionBox, mat4 a_mTransform)
//{
//	glm::vec3 _center = ((a_oCollisionBox.m_vMin + a_mTransform[3].xyz()) + (a_oCollisionBox.m_vMax + a_mTransform[3].xyz())) * 0.5f;
//	glm::vec3 _extents = (a_oCollisionBox.m_vMax + a_mTransform[3].xyz()) - (a_oCollisionBox.m_vMin + a_mTransform[3].xyz())) * 0.5f;
//
//	Gizmos::addAABB(_center, _extents, vec4(1, 0, 0, 1));
//}
