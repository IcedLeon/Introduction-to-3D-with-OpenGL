#ifndef _VERTEX_H_
#define _VERTEX_H_
//Lib
#define GLM_SWIZZLE
#include "glm/glm.hpp"

using glm::vec4;

class Vertex
{
public:
	vec4 m_vPos;
	vec4 m_vCol;

	Vertex() : m_vPos(vec4(0.0f, 0.0f, 0.0f, 0.0f)),
			   m_vCol(vec4(0.0f, 0.0f, 0.0f, 0.0f))
	{ }

	~Vertex() { }
};
#endif //!_VERTEX_H_