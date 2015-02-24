#include "FBXAnimationProject.h"
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "Gizmos.h"

FBXAnimationP::FBXAnimationP() : m_oFile(nullptr),
								 m_fTimer(NULL),
								 m_fAugmenter(NULL)
{

}

FBXAnimationP::~FBXAnimationP()
{
	
}

void FBXAnimationP::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);

	m_oProjection = m_oCamera.GetProjectionTransform(vec2(a_vScreenSize.x, a_vScreenSize.y));

	Gizmos::create();

	m_oTweek.CreateBar("Time");
	m_fAugmenter = 1.0f;
	TwAddVarRW(m_oTweek.GetMappedBar("Time"), "Time agumenter", TW_TYPE_FLOAT, &m_fAugmenter, "min=0.1 max=100 step=0.5");

	m_oFile = new FBXFile();
	m_oFile->load("./models/rigged/Pyro/pyro.fbx");
	m_oFile->initialiseOpenGLTextures();

	m_oShader.CreateShaderProgram(SKINNED_VERTEX_GLSL, SKINNED_FRAGMENT_GLSL);

	CreateGLMeshes(m_oFile);
}

void FBXAnimationP::CleanUpWin()
{
	m_oFile->unload();
	m_oShader.CleanUpProgram();
	Application::CleanUpWin();
}

void FBXAnimationP::Update(GLdouble a_fDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	//m_oWorld = m_oCamera.GetWorldTransform();

	Gizmos::clear();

	Gizmos::addTransform(mat4(1));

	vec4 _white(1);
	vec4 _black(0, 0, 0, 1);
	vec4 _red(1, 0, 0, 1);
	vec4 _green(0, 1, 0, 1);
	vec4 _blue(0, 0, 1, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? _blue : _black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? _red : _black);
	}

	m_fTimer += (float)a_fDeltaTime * m_fAugmenter;

	float _sinFreq = sinf(m_fTimer) * 0.5f + 0.5f;

	FBXSkeleton* _skely			= m_oFile->getSkeletonByIndex(0);
	FBXAnimation* _animatron	= m_oFile->getAnimationByIndex(0);

	//_skely->evaluate(_animatron, m_fTimer);

	EvalueteSkeleton(_animatron, _skely, m_fTimer);

	for (unsigned int bones = 0; bones < _skely->m_boneCount; ++bones)
	{
		_skely->m_nodes[bones]->updateGlobalTransform();
		mat4 _nodeGlobal = _skely->m_nodes[bones]->m_globalTransform;
		vec3 _nodePos = _nodeGlobal[3].xyz;

		Gizmos::addAABBFilled(_nodePos, vec3(2.0f), vec4(1, 0, 0, 1), &_nodeGlobal);

		if (_skely->m_nodes[bones]->m_parent != nullptr)
		{
			vec3 _parentPos = _skely->m_nodes[bones]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(_nodePos, _parentPos, vec4(0, 1, 0, 1));
		}
	}

	MoveCamera((float)a_fDeltaTime);
}

void FBXAnimationP::Draw()
{
	Use();
	m_oTweek.DrawTweek();
	mat4 _projView = m_oProjection * m_oView;
	Gizmos::draw(_projView);

	for (unsigned int i = 0; i < m_oMeshes.size(); ++i)
	{
		FBXMeshNode* _currMesh = m_oFile->getMeshByIndex(i);
		mat4 _worldTrans = _currMesh->m_globalTransform;

		GLuint _worldUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "World");
		glUniformMatrix4fv(_worldUni, 1, GL_FALSE, glm::value_ptr(_worldTrans));

		FBXMaterial* _meshMaterial = _currMesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _meshMaterial->textures[FBXMaterial::DiffuseTexture]->handle);
		glBindVertexArray(m_oMeshes[i].m_uiVAO);
		glDrawElements(GL_TRIANGLES, m_oMeshes[i].m_uiIndexCount, GL_UNSIGNED_INT, NULL);
	}
}

void FBXAnimationP::MoveCamera(float a_fDeltaTime)
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
}

void FBXAnimationP::Use()
{
	m_oShader.Use();

	GLuint _projUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Projection");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, glm::value_ptr(m_oProjection));

	GLuint _viewUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "View");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, glm::value_ptr(m_oView));


	GLuint _textDiffUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "diffuse_tex");
	glUniform1i(_textDiffUni, 0);

	FBXSkeleton* _skely = m_oFile->getSkeletonByIndex(0);
	//_skely->updateBones();
	UpdateBones(_skely);

	GLuint _bonesUni = glGetUniformLocation(m_oShader.GetShaderProgram(), "Bones");
	glUniformMatrix4fv(_bonesUni, _skely->m_boneCount, GL_FALSE, (float*)_skely->m_bones);
}

void FBXAnimationP::CreateGLMeshes(FBXFile* a_oFile)
{
	unsigned int _meshCount = a_oFile->getMeshCount();

	m_oMeshes.resize(_meshCount);

	for (unsigned int meshIndx = 0; meshIndx < _meshCount; ++meshIndx)
	{
		FBXMeshNode* _currMesh = a_oFile->getMeshByIndex(meshIndx);

		m_oMeshes[meshIndx].m_uiIndexCount = _currMesh->m_indices.size();

		glGenBuffers(1, &m_oMeshes[meshIndx].m_uiVBO);
		glGenBuffers(1, &m_oMeshes[meshIndx].m_uiIBO);
		glGenVertexArrays(1, &m_oMeshes[meshIndx].m_uiVAO);

		glBindVertexArray(m_oMeshes[meshIndx].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_oMeshes[meshIndx].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex) * _currMesh->m_vertices.size(),
			_currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oMeshes[meshIndx].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _currMesh->m_indices.size(),
			_currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //pos
		glEnableVertexAttribArray(1); //UV
		glEnableVertexAttribArray(2); //bone index
		glEnableVertexAttribArray(3); //bone weight

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);

		glBindVertexArray(NULL);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
}

void FBXAnimationP::EvalueteSkeleton(FBXAnimation* a_oAnimation, FBXSkeleton* a_oSkeleton, float a_fDeltaTime)
{
	float _FPS = 24.0f;
	int _currentFrame = (int)(a_fDeltaTime * _FPS);

	//iterate all the bones
	for (unsigned int trackidex = 0; trackidex < a_oAnimation->m_trackCount; ++trackidex)
	{
		//We avoid to indexing out of the array by wrapping from the start.
		int track_frame_count = a_oAnimation->m_tracks[trackidex].m_keyframeCount;
		int track_frame = _currentFrame % track_frame_count;

		//find the key frames that influce the bones
		FBXKeyFrame _currentKeyFrame = a_oAnimation->m_tracks[trackidex].m_keyframes[track_frame];
		FBXKeyFrame _nextKeyFrame = a_oAnimation->m_tracks[trackidex].m_keyframes[(track_frame + 1) % track_frame_count];

		float _time_since_frame_flip = a_fDeltaTime - (_currentFrame / _FPS);

		float _t = _time_since_frame_flip * _FPS;

		//interpolate between those keyframes to generate the matrix for current pose
		vec3 _newPos		= glm::mix(_currentKeyFrame.m_translation, _nextKeyFrame.m_translation, _t);
		vec3 _newScale		= glm::mix(_currentKeyFrame.m_scale, _nextKeyFrame.m_scale, _t);
		glm::quat _newRot	= glm::slerp(_currentKeyFrame.m_rotation, _nextKeyFrame.m_rotation, _t);

		mat4 local_transform = glm::translate(_newPos) * glm::toMat4(_newRot) * glm::scale(_newScale);
		//Getting the right bone for the given track
		unsigned int _boneIndex = a_oAnimation->m_tracks[trackidex].m_boneIndex;

		if (_boneIndex < a_oSkeleton->m_boneCount)
		{
			//set the fbx node local transform to match
			a_oSkeleton->m_nodes[_boneIndex]->m_localTransform = local_transform;
		}
	}
}

void FBXAnimationP::UpdateBones(FBXSkeleton* a_oSkeleton)
{
	//loop through the nodes int hte ckeleton
	for (unsigned int bone_index = 0; bone_index < a_oSkeleton->m_boneCount; ++bone_index)
	{
		//generate their global transforms
		int parent_index = a_oSkeleton->m_parentIndex[bone_index];

		if (parent_index == -1)
		{
			a_oSkeleton->m_bones[bone_index] = a_oSkeleton->m_nodes[bone_index]->m_localTransform;
		}
		else
		{
			a_oSkeleton->m_bones[bone_index] = a_oSkeleton->m_bones[parent_index] * a_oSkeleton->m_nodes[bone_index]->m_localTransform;
		}
	}

	for (unsigned int bone_index = 0; bone_index < a_oSkeleton->m_boneCount; ++bone_index)
	{
		a_oSkeleton->m_bones[bone_index] = a_oSkeleton->m_bones[bone_index] * a_oSkeleton->m_bindPoses[bone_index];
	}
	//multyply the global transform by the inverse bind pose
}