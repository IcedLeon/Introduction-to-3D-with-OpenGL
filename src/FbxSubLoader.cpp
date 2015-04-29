#include "FbxSubLoader.h"
#include "gl_core_4_4.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"

FbxSubLoader::FbxSubLoader() : m_oMeshFile(nullptr) { }

FbxSubLoader::~FbxSubLoader()
{
	m_oAllTheModels.clear();
	if (m_oMeshFile != nullptr)
		m_oMeshFile->unload();
}

bool FbxSubLoader::IsModelExist(const std::string& a_sModelID) const
{
	bool _found = false;

	if (m_oAllTheModels.count(a_sModelID) != NULL)
	{
		_found = true;
	}
	else
	{
		printf("<WARNING>: This model: %s, could not be found.\n", a_sModelID);
	}
	return _found;
}

std::string FbxSubLoader::ExtractNameOutOfFilePath(const char& a_rccFilePath) const
{
	std::string _toConvert(&a_rccFilePath);

	std::string _outString;

	size_t _found = _toConvert.find_last_of('/');

	if (_found != std::string::npos)
		_outString = _toConvert.substr(_found + 1);

	std::string _fbxExt = ".fbx";

	_found = _outString.find(_fbxExt);

	if (_found != std::string::npos)
	{
		_outString.replace(_outString.find(_fbxExt), _outString.length(), "");
	}
	else
	{
#define FBX_PATH_IO "<WARNING>: File extension do not match %s .\nPlease check again your source file: %s \n"
		printf(FBX_PATH_IO, _fbxExt.c_str(), a_rccFilePath);
	}

	return _outString;
}

void FbxSubLoader::SetModel(const std::string& a_sModelID, FBXFile& a_oData)
{
	m_oAllTheModels[a_sModelID] = &a_oData;
}

FBXFile* FbxSubLoader::GetModelByName(const std::string& a_sModelID) const
{
	FBXFile* _result = nullptr;
	if (IsModelExist(a_sModelID))
	{ 
		_result = m_oAllTheModels.find(a_sModelID)->second;
	}
	return _result;
}

void FbxSubLoader::LoadFileFromSrc(const char* a_pccFileName)
{
	if (m_oMeshFile != nullptr)
		m_oMeshFile->unload();
	else
		m_oMeshFile = new FBXFile();

	m_oMeshFile->load(a_pccFileName);
	m_oMeshFile->initialiseOpenGLTextures();

	unsigned int _meshCount = m_oMeshFile->getMeshCount();

	MeshDD* _newMeshData = new MeshDD();

	FBXMeshNode* _currNode;

	m_oMeshContainer.resize(m_oMeshContainer.size() + _meshCount);

	for (unsigned int meshIdx = 0; meshIdx < _meshCount; ++meshIdx)
	{ 
		_currNode = m_oMeshFile->getMeshByIndex(meshIdx);

		_newMeshData->SetModelTrans(_currNode->m_globalTransform);
		//m_oMeshContainer[meshIdx]->MESH_DATA.m_mMeshTrans = _currNode->m_globalTransform;
		_newMeshData->SetIndicesCount(_currNode->m_indices.size());
		//m_oMeshContainer[meshIdx]->SetIndicesCount(_currNode->m_indices.size());

		glGenVertexArrays(1, &_newMeshData->MESH_DATA.m_uiVAO);

		glGenBuffers(1, &_newMeshData->MESH_DATA.m_uiVBO);
		glGenBuffers(1, &_newMeshData->MESH_DATA.m_uiIBO);

		glBindVertexArray(_newMeshData->MESH_DATA.m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, _newMeshData->MESH_DATA.m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex) * _currNode->m_vertices.size(),
			_currNode->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _newMeshData->MESH_DATA.m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _currNode->m_indices.size(),
			_currNode->m_indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glEnableVertexAttribArray(8);

		glBindVertexArray(NULL);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		m_oMeshContainer[meshIdx] = *_newMeshData;
	}
	//vector<const char*>::iterator itr = a_lpccTextures.begin();
	//for (itr; itr != a_lpccTextures.end(); ++itr)
	//{
	//	m_uiDiffuse = TEXLOADER::LoadTexture("./models/rigged/Pyro/Pyro_D.tga");
	//	m_uiNormal = TEXLOADER::LoadTexture("./models/rigged/Pyro/Pyro_N.tga");
	//	m_uiSpecular = TEXLOADER::LoadTexture("./models/rigged/Pyro/Pyro_S.tga");
	//}
	std::string _ID = ExtractNameOutOfFilePath(*a_pccFileName);
	SetModel(_ID, *m_oMeshFile);
	delete _newMeshData;
}

void FbxSubLoader::LoadFileFromSrc(const char* a_pccFileName, std::vector<const char*> a_lpccTextures)
{

}

void FbxSubLoader::UnloadModel(const char* a_pccFileName)
{
	std::string _ID = ExtractNameOutOfFilePath(*a_pccFileName);
	if (IsModelExist(_ID))
	{
		std::map<std::string, FBXFile*>::iterator _it;
		_it = m_oAllTheModels.find(_ID);
		m_oAllTheModels.erase(_it);
	}
}

void FbxSubLoader::EvaluateSkely(float a_fDeltaT)
{
	float _fps = 24.0f;
	int _currFrame = (int)(a_fDeltaT * _fps);

	//Iterate bones
	for (unsigned int track = 0; track < m_oAnimatron->m_trackCount; ++track)
	{
		int _trackFrameCount = m_oAnimatron->m_tracks[track].m_keyframeCount;
		int _trackFrame = _currFrame % _trackFrameCount;

		FBXKeyFrame _currentKeyFrame = m_oAnimatron->m_tracks[track].m_keyframes[_trackFrame];
		FBXKeyFrame _nextKeyFrame = m_oAnimatron->m_tracks[track].m_keyframes[(_trackFrame + 1) % _trackFrameCount];

		float _timeSinceFrameFlip = a_fDeltaT - (_currFrame / _fps);

		float _time = _timeSinceFrameFlip * _fps;

		//Interpolation
		glm::vec3 _newPos = glm::mix(_currentKeyFrame.m_translation, _nextKeyFrame.m_translation, _time);
		glm::vec3 _newScale = glm::mix(_currentKeyFrame.m_scale, _nextKeyFrame.m_scale, _time);
		glm::quat _newRot = glm::slerp(_currentKeyFrame.m_rotation, _nextKeyFrame.m_rotation, _time);

		glm::mat4 _localTrans = glm::translate(_newPos) * glm::toMat4(_newRot) * glm::scale(_newScale);

		unsigned int _bones = m_oAnimatron->m_tracks[track].m_boneIndex;

		if (_bones < m_oSkely->m_boneCount)
		{
			m_oSkely->m_nodes[_bones]->m_localTransform = _localTrans;
		}
	}
}

void FbxSubLoader::UpdateSkely()
{
	for (unsigned int boneIdx = 0; boneIdx < m_oSkely->m_boneCount; ++boneIdx)
	{
		int _parentIdx = m_oSkely->m_parentIndex[boneIdx];

		if (_parentIdx == -1)
		{
			m_oSkely->m_bones[boneIdx] = m_oSkely->m_nodes[boneIdx]->m_localTransform;
		}
		else
		{
			m_oSkely->m_bones[boneIdx] = m_oSkely->m_bones[_parentIdx] * m_oSkely->m_nodes[boneIdx]->m_localTransform;
		}
	}

	for (unsigned int boneIdx = 0; boneIdx < m_oSkely->m_boneCount; ++boneIdx)
	{
		m_oSkely->m_bones[boneIdx] = m_oSkely->m_bones[boneIdx] * m_oSkely->m_bindPoses[boneIdx];
	}
}

void FbxSubLoader::UpdateMesh(ShaderProgram& a_uiProg, float a_fDeltaT)
{
	
	std::map<std::string, FBXFile*>::iterator itr = m_oAllTheModels.begin();
	
	for (itr; itr != m_oAllTheModels.end(); ++itr)
	{
		m_oMeshFile = itr->second;

		m_fTimer += a_fDeltaT;

		float _freq = sinf(m_fTimer) * 0.5f + 0.5f;

		m_oSkely = m_oMeshFile->getSkeletonByIndex(0);
		a_uiProg.Use();

		UpdateSkely();

		a_uiProg.SetUniform("Bones", *m_oSkely->m_bones, m_oSkely->m_boneCount);

		a_uiProg.Disable();

		m_oAnimatron = m_oMeshFile->getAnimationByIndex(0);

		EvaluateSkely(m_fTimer);
	}
}

void FbxSubLoader::Draw(ShaderProgram& a_uiProg, glm::mat4 a_mTranslate)
{
	std::map<std::string, FBXFile*>::iterator itr = m_oAllTheModels.begin();

	for (itr; itr != m_oAllTheModels.end(); ++itr)
	{
		m_oMeshFile = itr->second;
		a_uiProg.Use();
		for (unsigned int i = 0; i < m_oMeshContainer.size(); ++i)
		{
			FBXMeshNode* _currMesh = m_oMeshFile->getMeshByIndex(i);

			m_oMeshContainer[i].MESH_DATA.m_mMeshTrans = _currMesh->m_globalTransform * a_mTranslate * glm::scale(vec3(0.2, 0.2, 0.2));
			a_uiProg.SetUniform("Model", m_oMeshContainer[i].MESH_DATA.m_mMeshTrans);
			FBXMaterial* _currMeshMat = _currMesh->m_material;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::DiffuseTexture]->handle);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::NormalTexture]->handle);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::SpecularTexture]->handle);

			glBindVertexArray(m_oMeshContainer[i].GetVAO());
			glDrawElements(GL_TRIANGLES, m_oMeshContainer[i].GetIndicesCount(), GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}
		a_uiProg.Disable();
	}
}

//void FbxSubLoader::TestMultymap(const char* a_pccTestName)
//{
//	FBXFile* _meshFile = new FBXFile();
//
//	_meshFile->load(a_pccTestName);
//	_meshFile->initialiseOpenGLTextures();
//
//	unsigned int _meshCount = _meshFile->getMeshCount();
//
//	MeshDD* _newMeshData = new MeshDD();
//
//	FBXMeshNode* _currNode;
//
//	std::vector<MeshDD> _tempCont;
//
//	_tempCont.resize(_meshCount);
//
//	for (unsigned int meshIdx = 0; meshIdx < _meshCount; ++meshIdx)
//	{
//		_currNode = _meshFile->getMeshByIndex(meshIdx);
//
//		_newMeshData->SetModelTrans(_currNode->m_globalTransform);
//		//m_oMeshContainer[meshIdx]->MESH_DATA.m_mMeshTrans = _currNode->m_globalTransform;
//		_newMeshData->SetIndicesCount(_currNode->m_indices.size());
//		//m_oMeshContainer[meshIdx]->SetIndicesCount(_currNode->m_indices.size());
//
//		glGenVertexArrays(1, &_newMeshData->MESH_DATA.m_uiVAO);
//
//		glGenBuffers(1, &_newMeshData->MESH_DATA.m_uiVBO);
//		glGenBuffers(1, &_newMeshData->MESH_DATA.m_uiIBO);
//
//		glBindVertexArray(_newMeshData->MESH_DATA.m_uiVAO);
//
//		glBindBuffer(GL_ARRAY_BUFFER, _newMeshData->MESH_DATA.m_uiVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex) * _currNode->m_vertices.size(),
//			_currNode->m_vertices.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _newMeshData->MESH_DATA.m_uiIBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _currNode->m_indices.size(),
//			_currNode->m_indices.data(), GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
//		glEnableVertexAttribArray(8);
//
//		glBindVertexArray(NULL);
//
//		glBindBuffer(GL_ARRAY_BUFFER, NULL);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
//		_tempCont[meshIdx] = *_newMeshData;
//	}
//
//	std::string _ID = ExtractNameOutOfFilePath(*a_pccTestName);
//
//	m_oMultyMap.insert(std::make_pair(_ID, std::map<FBXFile*, std::vector<MeshDD>>()));
//	m_oMultyMap[_ID].insert(std::make_pair(_meshFile, _tempCont));
//
//	delete _newMeshData;
//}
//
//void FbxSubLoader::UpdateMulty(ShaderProgram& a_uiProg, float a_fDeltaT)
//{
//	FBXFile* _meshFile = new FBXFile();
//
//	for (std::map<std::string, std::map<FBXFile*, std::vector<MeshDD>>>::iterator itr = m_oMultyMap.begin();
//		itr != m_oMultyMap.end(); ++itr)
//	{
//		for (std::map<FBXFile*, std::vector<MeshDD>>::iterator itr2 = itr->second.begin();
//			itr2 != itr->second.end(); ++itr2)
//		{
//
//			_meshFile = itr2->first;
//
//			m_fTimer += a_fDeltaT;
//
//			float _freq = sinf(m_fTimer) * 0.5f + 0.5f;
//
//			m_oSkely = _meshFile->getSkeletonByIndex(0);
//			a_uiProg.Use();
//			//UpdateSkely();
//			m_oSkely->updateBones();
//			a_uiProg.SetUniform("Bones", *m_oSkely->m_bones, m_oSkely->m_boneCount);
//			a_uiProg.Disable();
//
//			m_oAnimatron = _meshFile->getAnimationByIndex(0);
//
//			EvaluateSkely(m_fTimer);
//		}
//	}
//}
//
//void FbxSubLoader::DrawTestMulty(ShaderProgram& a_uiProg, glm::mat4 a_mTranslate)
//{
//	FBXFile* _meshFile = new FBXFile();
//	for (std::map<std::string, std::map<FBXFile*, std::vector<MeshDD>>>::iterator itr = m_oMultyMap.begin();
//		itr != m_oMultyMap.end(); ++itr)
//	{
//		for (std::map<FBXFile*, std::vector<MeshDD>>::iterator itr2 = itr->second.begin();
//			itr2 != itr->second.end(); ++itr2)
//		{
//			a_uiProg.Use();
//			_meshFile = itr2->first;
//			for (unsigned int i = 0; i < (*itr2).second.size(); ++i)
//			{
//				FBXMeshNode* _currMesh = _meshFile->getMeshByIndex(i);
//
//				(*itr2).second[i].MESH_DATA.m_mMeshTrans = _currMesh->m_globalTransform * a_mTranslate * glm::scale(vec3(0.2, 0.2, 0.2));
//				a_uiProg.SetUniform("Model", (*itr2).second[i].MESH_DATA.m_mMeshTrans);
//				FBXMaterial* _currMeshMat = _currMesh->m_material;
//
//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::DiffuseTexture]->handle);
//
//				glActiveTexture(GL_TEXTURE1);
//				glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::NormalTexture]->handle);
//
//				glActiveTexture(GL_TEXTURE2);
//				glBindTexture(GL_TEXTURE_2D, _currMeshMat->textures[FBXMaterial::SpecularTexture]->handle);
//
//				glBindVertexArray((*itr2).second[i].GetVAO());
//				glDrawElements(GL_TRIANGLES, (*itr2).second[i].GetIndicesCount(), GL_UNSIGNED_INT, NULL);
//				glBindVertexArray(0);
//			}
//			a_uiProg.Disable();
//		}
//	}
//}