#ifndef _GIZZLE_H_
#define _GIZZLE_H_
///Lib
#include "gl_core_4_4.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <map>
#include <string>

//Using macro
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::map;
//Macro
#define PI 3.141592653589793
#define TWOPI 6.2831853071795862
#define TWOPI_F 6.2831853f

class Drawable
{
protected:
	struct Vertex
	{
		vec4* m_vPosition;
		vec4* m_vNormals;
		vec4* m_vTangents;
		vec2* m_vUVCoords;
	};

	struct
	{
		mat4 m_mModelTrans;
		GLuint m_uiIndexCount;
		int m_iIDN;
	} Data;


	struct
	{
		GLuint m_uiVAO;
		GLuint m_uiVBO;
		GLuint m_uiIBO;
	} Handle;

public:
	Drawable();
	~Drawable();

	virtual void Render() const = NULL;

	void SetObjTransform(mat4 a_mNewTransform);
	mat4 GetObjTransform() const;
	void SetID(int a_iIDN);
	int GetID() const;
	GLuint GetIndices() const;

	GLuint GetVAO() const;
	GLuint GetVBO() const;
	GLuint GetIBO() const;
};
  //////////////////////
 //
//
class Plane : public Drawable
{
private:
	int m_iFaces;
public:
	Plane ();
	~Plane();

	void GenPlane(glm::vec2 a_vDimensions, glm::ivec2 a_vDivisions, float a_fSMax, float a_fTMax);

	void Render() const override;
};

class Cube : public Drawable
{
public:
	Cube();
	~Cube();

	void GenCube();
	virtual void Render() const override;
};

class Sphere : public Drawable
{
private:
	float m_fRadius;
	float m_fSlices;
	float m_fStacks;
	GLuint m_uiVertices;

	void GenerateVerts(float* a_fVerticies, float* a_fNormals, float* a_fUVCoord, unsigned int* a_uiElements);

public:
	Sphere();
	~Sphere();

	void GenSphere(float a_fRadius, int a_iSlices, int a_iStacks);

	void Render() const override;
};

class Torus : public Drawable
{
private:
	int m_iFaces;
	int m_iRings;
	int m_iSides;

	void GenerateVerts(float* a_fVerticies, float* a_fNormals, float* a_fUVCoord, 
		unsigned int* a_uiElements, float a_fOuterRadius, float a_fInnerRadius);

public:
	Torus();
	~Torus();

	void GenTorus(float a_fOuterRadius, float a_fInnerRadius, int a_iNumOfSides, int a_iNumOfRings);

	void Render() const;
};

using std::string;

#define MAX_OBJ 100

class Gizzle
{
private:
	map<string, Drawable*> m_moActiveObjects;
	enum
	{
		PLANE,
		CUBE,
		SPHERE,
		TORUS,
		MESH,
		SKINNED_MESH,
		NONE
	} Objects;
	string m_sAlphaID;
	int m_iActiveObjects,
		m_iNumOfPlane,
		m_iNumOfCube,
		m_iNumOfSphere,
		m_iNumOfTorus,
		m_iNumOfMesh;
	static int m_iID;

	void AddPlane(vec2 a_vDimensions, glm::ivec2 a_vDivisions, float a_fSMax, float a_fTMax);
	void AddCube();
	void AddSphere(float a_fRadius, int a_iSlices, int a_iStacks);
	void AddTorus(float a_fOuterRadius, float a_fInnerRadius, int a_iNumOfSides, int a_iNumOfRings);
	void AddMesh(const char* a_pccFileName, bool a_bReCenterMesh = false, bool a_bLoadTex = false, bool a_bGenTangents = false) {}
	void AddSkinnedMesh(){}

	void GenID();

public:
	Gizzle();
	~Gizzle();

	void CreateObject(int a_iObjID, 
		float* a_fFloatValues = nullptr, 
		int* a_iIntValues = nullptr, 
		bool* a_bOptions = nullptr, 
		const char* a_pccFileName = nullptr);

	void DeleteObject(int a_iObjID) {}

	void Draw();
};
#endif //!_GIZZLE_H_