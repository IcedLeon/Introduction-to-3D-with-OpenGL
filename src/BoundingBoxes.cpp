#include "BoundingBoxes.h"

void AABB::Reset()
{
	m_vMin.x = m_vMin.y = m_vMin.z = 1e37f;
	m_vMax.x = m_vMax.y = m_vMax.z = -1e37f;
}

void AABB::Fit(const std::vector<glm::vec3>& a_vPoints)
{
	for each (glm::vec3 p in a_vPoints)
	{
		if (p.x < m_vMin.x) 
			m_vMin.x = p.x;
		if (p.y < m_vMin.y) 
			m_vMin.y = p.y;
		if (p.z < m_vMin.z) 
			m_vMin.z = p.z;

		if (p.x > m_vMax.x) 
			m_vMax.x = p.x;
		if (p.y > m_vMax.y) 
			m_vMax.y = p.y;
		if (p.z > m_vMax.z) 
			m_vMax.z = p.z;
	}

}

void BoundingSphere::Fit(const std::vector<glm::vec3>& a_vPoints)
{
	glm::vec3 _min(1e37f), _max(-1e37f);

	for each (glm::vec3 p in a_vPoints)
	{
		if (p.x < _min.x) 
			_min.x = p.x;
		if (p.y < _min.y) 
			_min.y = p.y;
		if (p.z < _min.z) 
			_min.z = p.z;

		if (p.x > _max.x) 
			_max.x = p.x;
		if (p.y > _max.y) 
			_max.y = p.y;
		if (p.z > _max.z) 
			_max.z = p.z;

		m_vCentre = (_min + _max) * 0.5f;
		m_fRadius = glm::distance(_min, m_vCentre);
	}
}

int BoundingSphere::CheckCollision(glm::vec4 a_vPlane)
{
	float _dotResult = glm::dot(glm::vec3(a_vPlane), m_vCentre) + a_vPlane.w;

	if (_dotResult > m_fRadius)
	{
		printf("Front \n");
		return 1;
	}
	else if (_dotResult < -m_fRadius)
	{
		printf("Back \n");
		return -1;
	}
	else
	{
		printf("On the plane \n");
		return 0;
	}
}