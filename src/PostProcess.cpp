//#include "PostProcess.h"
//#include <GLFW\glfw3.h>
//#include "Camera.h"
//#include "Gizmos.h"
//
//
//PostProcess::PostProcess()
//{
//
//}
//
//PostProcess::~PostProcess()
//{
//
//}
//
//void PostProcess::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
//{
//	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
//	m_oProjection = m_oCamera.GetProjectionTransform(vec2(a_vScreenSize.x, a_vScreenSize.y));
//
//	Gizmos::create();
//
//	GenerateFrameBuffers();
//	GenScreenSpaceQuad();
//
//	m_oShader.CreateShaderProgram(RENDERING_VERTEX_GLSL, RENDERING_FRAGMENT_GLSL);
//}
//
//void PostProcess::Update(GLdouble a_fDeltaTime)
//{
//	Gizmos::clear();
//	Gizmos::addTransform(mat4(1));
//
//	m_oView = m_oCamera.GetViewTransform();
//	m_oWorld = m_oCamera.GetWorldTransform();
//
//	vec4 _white(1);
//	vec4 _black(0, 0, 0, 1);
//	vec4 _red(1, 0, 0, 1);
//	vec4 _green(0, 1, 0, 1);
//	vec4 _blue(0, 0, 1, 1);
//
//	for (int i = 0; i <= 20; ++i)
//	{
//		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
//			i == 10 ? _blue : _black);
//		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
//			i == 10 ? _red : _black);
//	}
//
//	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 0, 1, 1));
//
//	MoveCamera((float)a_fDeltaTime);
//}
//
//void PostProcess::CleanUpWin()
//{
//	Gizmos::destroy();
//	m_oShader.CleanUpProgram();
//	Application::CleanUpWin();
//}
//
//void PostProcess::Draw()
//{
//	mat4 _projView = m_oProjection * m_oView;
//	//Frame buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO);
//	glViewport(0, 0, m_iNewSize, m_iNewSize);
//	glClearColor(0.75f, 0.75f, 0.75f, 0.2f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	vec4 _plane = vec4(0, 0, 1, -5);
//	mat4 _worldRef = m_oWorld;
//
//	vec3 _reflected = glm::reflect(-_worldRef[2].xyz(), _plane.xyz());
//	_reflected = glm::normalize(_reflected);
//
//	float _dist = (glm::dot((_plane.xyz() * _plane.w) - _worldRef[3].xyz(), _plane.xyz())) /
//		(glm::dot(_plane.xyz(), -_worldRef[2].xyz()));
//
//	vec3 _intersecting = _worldRef[3].xyz() + (-_worldRef[2].xyz()) * _dist;
//
//	_worldRef[3].xyz = _intersecting - _reflected * _dist;
//
//	vec3 _up = vec3(0, 1, 0);
//
//	_worldRef[2].xyz = -_reflected;
//	_worldRef[0].xyz = glm::normalize(glm::cross(_worldRef[2].xyz(), _up));
//	_worldRef[1].xyz = glm::normalize(glm::cross(_worldRef[0].xyz(), _worldRef[2].xyz()));
//
//	mat4 _view = glm::inverse(_worldRef);
//	mat4 _projViewRef = m_oProjection * _view;
//
//	m_oShader.Use();
//	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
//	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));
//
//	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
//	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(_view));
//
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//
//	glBindVertexArray(m_oPlane.m_uiVAO);
//	glDrawElements(GL_TRIANGLES, m_oPlane.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
//
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//
//	glDepthFunc(GL_GREATER);
//	Gizmos::draw(_projViewRef);
//	glDepthFunc(GL_LESS);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
//	//back to the back_buffer.
//	glViewport(0, 0, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
//	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	Gizmos::draw(_projView);
//
//	//Use();
//
//	GLint _projViewReflected = glGetUniformLocation(m_oShader.GetShaderProgram(), "PVReflected");
//	glUniformMatrix4fv(_projViewReflected, 1, GL_FALSE, glm::value_ptr(_projViewRef));
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_uiFBO_T);
//	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Diffuse");
//	glUniform1i(m_uiFBO_T, 0);
//
//	glBindVertexArray(m_oPlane.m_uiVAO);
//	glDrawElements(GL_TRIANGLES, m_oPlane.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
//}
//
//void PostProcess::MoveCamera(float a_fDeltaTime)
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
//}
//
//void PostProcess::Use()
//{
//	m_oShader.Use();
//
//	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
//	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));
//
//	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
//	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));
//
//	//GLuint _worldUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Model");
//	//glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oWorld));
//}
//
//void PostProcess::GenerateFrameBuffers()
//{
//	//Frame buffer generation
//	glGenFramebuffers(1, &m_uiFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO);
//	//Frame buffer texture generation
//	glGenTextures(1, &m_uiFBO_T);
//	glBindTexture(GL_TEXTURE_2D, m_uiFBO_T);
//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	//Funny result
//	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiTexture, NULL);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiFBO_T, NULL);
//	//Frame buffer depth generation
//	glGenRenderbuffers(1, &m_uiFBO_D);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_uiFBO_D);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (int)m_vScreenSize.x, (int)m_vScreenSize.y);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uiFBO_D);
//	//
//	GLenum _drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, _drawBuffers);
//	//
//	GLenum _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (_status != GL_FRAMEBUFFER_COMPLETE)
//		printf("ERROR: <FRAME BUFFER IS INCOMPLETE> \n");
//	//
//	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
//}
//
//void PostProcess::GenScreenSpaceQuad()
//{
//
//	vec2 _halfTexel = 1.0f / vec2(m_vScreenSize.x, m_vScreenSize.y) * 0.5f;
//
//	float _vertexData[] =
//	{
//		-1, -1, 0, 1,	_halfTexel.x, _halfTexel.y,
//		 1, -1, 0, 1,	1-_halfTexel.x, _halfTexel.y,
//		 1,  1, 0, 1,	1-_halfTexel.x, 1-_halfTexel.y,
//		-1,  1, 0, 1,	_halfTexel.x, 1-_halfTexel.y,
//	};
//
//	GLuint _indexData[] =
//	{
//		0, 1, 2,
//		0, 2, 3,
//	};
//
//	m_oQuad.m_uiIndexCount = 6;
//
//	glGenVertexArrays(1, &m_oQuad.m_uiVAO);
//	glBindVertexArray(m_oQuad.m_uiVAO);
//
//	glGenBuffers(1, &m_oQuad.m_uiVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, m_oQuad.m_uiVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertexData), _vertexData, GL_STATIC_DRAW);
//
//	glGenBuffers(1, &m_oQuad.m_uiIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oQuad.m_uiIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indexData), _indexData, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, NULL);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));
//
//	glBindVertexArray(NULL);
//	glBindBuffer(GL_ARRAY_BUFFER, NULL);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
//}