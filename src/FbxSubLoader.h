#ifndef _FBXSUBLOADERH_
#define _FBXSUBLOADER_H_
//Lib
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FBXFile.h"
#include <vector>
#include <cstdint>

/* Mesh detail class */
class MeshDataDetail
{
public:
	struct
	{
		glm::mat4 m_mMeshTrans;
		std::uint32_t m_uiVAO;
		std::uint32_t m_uiVBO;
		std::uint32_t m_uiIBO;
	} MESH_DATA;
private:
	struct
	{
		std::uint32_t m_uiIndexCount;
		std::uint32_t m_uiIndexOffset;
		std::uint32_t m_uiTriangleCount;
	} COUNT;

public:
	MeshDataDetail() 
	{
		MESH_DATA.m_mMeshTrans = glm::mat4(1);
		MESH_DATA.m_uiVAO = NULL;
		MESH_DATA.m_uiVBO = NULL;
		MESH_DATA.m_uiIBO = NULL;
		/******************************/
		COUNT.m_uiIndexCount	= NULL;
		COUNT.m_uiIndexOffset	= NULL;
		COUNT.m_uiTriangleCount = NULL;
	}

	MeshDataDetail(const MeshDataDetail& a_Other)
	{
		MESH_DATA.m_mMeshTrans	= a_Other.MESH_DATA.m_mMeshTrans;
		MESH_DATA.m_uiVAO		= a_Other.MESH_DATA.m_uiVAO;
		MESH_DATA.m_uiVBO		= a_Other.MESH_DATA.m_uiVBO;
		MESH_DATA.m_uiIBO		= a_Other.MESH_DATA.m_uiIBO;
		/******************************/
		COUNT.m_uiIndexCount	= a_Other.COUNT.m_uiIndexCount;
		COUNT.m_uiIndexOffset	= a_Other.COUNT.m_uiIndexOffset;
		COUNT.m_uiTriangleCount = a_Other.COUNT.m_uiTriangleCount;
	}

	~MeshDataDetail() {}
	/* Data set/return */
	void SetModelTrans(const glm::mat4& a_mTrans) { this->MESH_DATA.m_mMeshTrans = a_mTrans; }
	glm::mat4 GetModelTrans() const { return this->MESH_DATA.m_mMeshTrans; }
	std::uint32_t GetVAO() { return this->MESH_DATA.m_uiVAO; }
	std::uint32_t GetVBO() { return this->MESH_DATA.m_uiVBO; }
	std::uint32_t GetIBO() { return this->MESH_DATA.m_uiIBO; }
	/* Count set/return */
	void SetIndicesCount(std::uint32_t a_uiIndexCount) { this->COUNT.m_uiIndexCount = a_uiIndexCount; }
	std::uint32_t GetIndicesCount() const { return this->COUNT.m_uiIndexCount; }
	void SetIndicesOffset(std::uint32_t a_uiIndexOffset) { this->COUNT.m_uiIndexOffset = a_uiIndexOffset; }
	std::uint32_t GetIndicesOffset() const { return this->COUNT.m_uiIndexOffset; }
	void SetTriangleCount(std::uint32_t a_uiTriCount) { this->COUNT.m_uiTriangleCount = a_uiTriCount; }
	std::uint32_t GetTriangleCount() const { return this->COUNT.m_uiTriangleCount; }
};

typedef MeshDataDetail MeshDD;

class ShaderProgram;

class FbxSubLoader
{
private:
	FBXFile* m_oMeshFile;
	FBXSkeleton* m_oSkely;
	FBXAnimation* m_oAnimatron;
	std::map<std::string, FBXFile*> m_oAllTheModels;
	std::vector<MeshDD> m_oMeshContainer;
	//std::map<std::string, std::map<FBXFile*, std::vector<MeshDD>>> m_oMultyMap;
	float m_fTimer;

	unsigned int m_uiDiffuse;
	unsigned int m_uiNormal;
	unsigned int m_uiSpecular;

	bool IsModelExist(const std::string& a_sModelID) const;

	std::string ExtractNameOutOfFilePath(const char& a_rccFilePath) const;

	void SetModel(const std::string& a_sModelID, FBXFile& a_oData);

public:
	FbxSubLoader();
	~FbxSubLoader();
	
	FBXFile* GetModelByName(const std::string& a_sModelID) const;

	void LoadFileFromSrc(const char* a_pccFileName);
	void LoadFileFromSrc(const char* a_pccFileName, std::vector<const char*> a_lpccTextures);
	void UnloadModel(const char* a_pccFileName);
	void EvaluateSkely(float a_fDeltaT);
	void UpdateSkely();

	void UpdateMesh(ShaderProgram& a_uiProg, float a_fDeltaT);
	void Draw(ShaderProgram& a_uiProg, glm::mat4 a_mTranslate);

	//void TestMultymap(const char* a_pccTestName);
	//void UpdateMulty(ShaderProgram& a_uiProg, float a_fDeltaT);
	//void DrawTestMulty(ShaderProgram& a_uiProg, glm::mat4 a_mTranslate);
};

#endif //!_FBXSUBLOADER_H_