#ifndef _BOUNDINGBOXES_H_
#define _BOUNDINGBOXES_H_
//Lib
#include "gl_core_4_4.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>

using std::vector;

class AABB
{
public:
	glm::vec3 m_vMin;
	glm::vec3 m_vMax;

	AABB() : m_vMin(glm::vec3(0)),
			 m_vMax(glm::vec3(0)) { }
	~AABB() { }

	void Reset();
	void Fit(const vector<glm::vec3>& a_vPoints);
	void GenerateAABB(glm::vec3* a_vPos, GLuint a_uiCount, GLuint a_uiStride = NULL);
};

class BoundingSphere
{
public:
	glm::vec3 m_vCentre;
	float m_fRadius;

	BoundingSphere() : m_vCentre(glm::vec3(0)),
					   m_fRadius(NULL) {}
	~BoundingSphere() {}

	void Fit(const vector<glm::vec3>& a_vPoints);

	int CheckCollision(glm::vec4 a_vPlane);
};

#endif //!_BOUNDINGBOXES_H_