#include "Gizzle.h"

  //////////////////////////
 //  Drawable base class //
//////////////////////////
Drawable::Drawable()
{
	Data.m_mModelTrans		= mat4(1.0f);
	Data.m_uiIndexCount		= NULL;
	Data.m_iIDN				= -1;

	Handle.m_uiVAO = NULL;
	Handle.m_uiVBO = NULL;
	Handle.m_uiIBO = NULL;
}

Drawable::~Drawable()
{

}

void Drawable::SetObjTransform(mat4 a_mNewTransform)
{
	this->Data.m_mModelTrans = a_mNewTransform;
}

mat4 Drawable::GetObjTransform() const
{
	return this->Data.m_mModelTrans;
}

void Drawable::SetID(int a_iIDN)
{
	this->Data.m_iIDN = a_iIDN;
}

int Drawable::GetID() const
{
	return this->Data.m_iIDN;
}

GLuint Drawable::GetVAO() const
{
	return this->Handle.m_uiVAO;
}

GLuint Drawable::GetVBO() const
{
	return this->Handle.m_uiVBO;
}

GLuint Drawable::GetIBO() const
{
	return this->Handle.m_uiIBO;
}

GLuint Drawable::GetIndices() const
{
	return this->Data.m_uiIndexCount;
}

//////////////////////////////
//  Drawable derived class //
////////////////////////////

//Plane
Plane::Plane() : Drawable(),
				 m_iFaces(NULL)
{

}

Plane::~Plane()
{

}

void Plane::GenPlane(glm::vec2 a_vDimensions, glm::ivec2 a_vDivisions, float a_fSMax, float a_fTMax)
{
	//Calculate the number of faces (division) of this plane.
	m_iFaces			= a_vDivisions.x * a_vDivisions.y;
	//Readbility improvement.
	//Total number of vertecies required to form this plane.
	GLuint _totalVertex = (a_vDivisions.x + 1) * (a_vDivisions.y + 1);
	Vertex* _vertexData = new Vertex[_totalVertex];
	//The toatl numner of indices of this plane.
	GLuint _totalIndex	= m_iFaces * 6;
	//Assign data to be later passed to our handles.
	_vertexData->m_vPosition = new vec4[_totalVertex];
	_vertexData->m_vNormals  = new vec4[_totalVertex];
	_vertexData->m_vTangents = new vec4[_totalVertex];
	_vertexData->m_vUVCoords = new vec2[_totalVertex];
	//Assing space to hold the index coordinates.
	GLuint* _indexData		= new GLuint[_totalIndex];
	//So we can call the right ammount of data out form the glDraw.
	Data.m_uiIndexCount = _totalIndex;
	//Other Readbility improvements
	//Half with and height. This would allow us to always center the plane to the 0x0 position.
	float _halfWidth	= a_vDimensions.x / 2.0f;
	float _halfHeight	= a_vDimensions.y / 2.0f;
	float _factorI		= (float)(a_vDimensions.y / a_vDivisions.y);
	float _factorJ		= (float)(a_vDimensions.x / a_vDivisions.x);
	float _uvI			= (float)(a_fSMax / a_vDivisions.y);
	float _uvJ			= (float)(a_fTMax / a_vDivisions.x);
	float _x, _z;
	//Create vertecies
	int _normVert = 0;
	int _texIdx	  = 0;
	for (int i = 0; i < a_vDivisions.y; ++i)
	{
		_z = _factorI * i - _halfHeight;
		for (int j = 0; j < a_vDivisions.x; ++j)
		{
			_x = _factorJ * j - _halfWidth;
			int _precisionIdx = j * (a_vDivisions.x + 1) + i;
			_vertexData->m_vPosition[_precisionIdx] = vec4(_x, 0.0f, _z, 1.0f);
			_vertexData->m_vNormals[_precisionIdx] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
			_vertexData->m_vTangents[_precisionIdx] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
			float _U = j * _uvI;
			float _V = i * _uvJ;
			_vertexData->m_vUVCoords[_precisionIdx] = vec2(_U, _V);
		}
	}
	unsigned int _rowStart;
	unsigned int _newRowStart;
	int _idx = 0;
	for (int i = 0; i < a_vDivisions.y; ++i)
	{
		_rowStart = i * (a_vDivisions.x + 1);
		_newRowStart = (i + 1) * (a_vDivisions.y + 1);
		for (int j = 0; j < a_vDivisions.x; ++j)
		{
			_indexData[_idx++] = _rowStart	  + j;
			_indexData[_idx++] = _newRowStart + j;
			_indexData[_idx++] = _newRowStart + j + 1;
			_indexData[_idx++] = _rowStart	  + j;
			_indexData[_idx++] = _newRowStart + j + 1;
			_indexData[_idx++] = _rowStart    + j + 1;
		}
	}
	//Generate buffers and binding
	//Vertex array
	glGenVertexArrays(1, &Handle.m_uiVAO);
	glBindVertexArray(Handle.m_uiVAO);
	//Vertex
	glGenBuffers(1, &Handle.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, Handle.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _totalVertex, _vertexData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0); //Position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));
	glEnableVertexAttribArray(1); //Normals											   
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));
	glEnableVertexAttribArray(2); //Tangents										   
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));
	glEnableVertexAttribArray(3); //Uv coordinates
	//Index
	glGenBuffers(1, &Handle.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _totalIndex, _indexData, GL_STATIC_DRAW);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	delete[] _vertexData;
	delete[] _indexData;
}

void Plane::Render() const
{
	glBindVertexArray(this->Handle.m_uiVAO);
	glDrawElements(GL_TRIANGLES, this->Data.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}

//Cube
Cube::Cube() : Drawable()
{

}

Cube::~Cube()
{

}

void Cube::GenCube()
{
	float _side = 1.0f;
	float _halfSide = _side / 2.0f;

	const unsigned int _totalIndex = 24 * 3;

	float* _vertexData = new float[_totalIndex]
	{
		//Front
		-_halfSide, -_halfSide, _halfSide,
			_halfSide, -_halfSide, _halfSide,
			_halfSide, _halfSide, _halfSide,
			-_halfSide, _halfSide, _halfSide,
			//Right
			_halfSide, -_halfSide, _halfSide,
			_halfSide, -_halfSide, -_halfSide,
			_halfSide, _halfSide, -_halfSide,
			_halfSide, _halfSide, _halfSide,
			//Back
			-_halfSide, -_halfSide, -_halfSide,
			-_halfSide, _halfSide, -_halfSide,
			_halfSide, _halfSide, -_halfSide,
			_halfSide, -_halfSide, -_halfSide,
			//Left
			-_halfSide, -_halfSide, _halfSide,
			-_halfSide, _halfSide, _halfSide,
			-_halfSide, _halfSide, -_halfSide,
			_halfSide, -_halfSide, -_halfSide,
			//Bottom
			-_halfSide, -_halfSide, _halfSide,
			-_halfSide, -_halfSide, -_halfSide,
			_halfSide, -_halfSide, -_halfSide,
			_halfSide, -_halfSide, _halfSide,
			//Top
			-_halfSide, _halfSide, _halfSide,
			_halfSide, _halfSide, _halfSide,
			_halfSide, _halfSide, -_halfSide,
			-_halfSide, _halfSide, -_halfSide,
	};

	float _normalData[_totalIndex] =
	{
		// Front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		// Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// Left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// Bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// Top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	const unsigned int _texIdx = 24 * 2;
	float _texData[_texIdx] =
	{
		// Front
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Right
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Back
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Left
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Bottom
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Top
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	GLuint _indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	glGenVertexArrays(1, &Handle.m_uiVAO);
	glBindVertexArray(Handle.m_uiVAO);

	glGenBuffers(1, &Handle.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, Handle.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, _totalIndex * sizeof(float), _vertexData, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(0); //Pos
	glBufferData(GL_ARRAY_BUFFER, _totalIndex * sizeof(float), _normalData, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(1); //Normal
	glBufferData(GL_ARRAY_BUFFER, _texIdx * sizeof(float), _texData, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(3); //UV

	glGenBuffers(1, &Handle.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), _indices, GL_STATIC_DRAW);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	delete[] _vertexData;
	delete[] _normalData;
	delete[] _texData;
	delete[] _indices;
}

void Cube::Render() const
{
	glBindVertexArray(Handle.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, ((GLubyte*)NULL + (0)));
}

Sphere::Sphere() : Drawable(),
				   m_fRadius(NULL),
				   m_fSlices(NULL),
				   m_fStacks(NULL),
				   m_uiVertices(NULL)
{

}

Sphere::~Sphere()
{
	
}

void Sphere::GenerateVerts(float* a_fVerticies, float* a_fNormals, float* a_fUVCoord, unsigned int* a_uiElements)
{
	//Gen position and Normals
	GLfloat _theta,
		    _phi;
	GLfloat _thetaFactor = (2.0f * (float)PI) / m_fSlices;
	GLfloat _phiFactor = (float)PI / m_fStacks;
	GLfloat _normX, _normY, _normZ, _s, _t;
	GLuint _idx = 0, _tIdx = 0;
	for (int i = 0; i < m_fSlices; ++i)
	{
		_theta = i * _thetaFactor;
		_s = (GLfloat)i / m_fSlices;
		for (int j = 0; j < m_fStacks; j++)
		{
			_phi = j * _phiFactor;
			_t = (GLfloat)j / m_fStacks;

			_normX = sinf(_phi) * cosf(_theta);
			_normY = sinf(_phi) * sinf(_theta);
			_normZ = cosf(_phi);
			
			a_fVerticies[_idx] = m_fRadius * _normX;
			a_fVerticies[_idx + 1] = m_fRadius * _normY;
			a_fVerticies[_idx + 2] = m_fRadius * _normZ;
			a_fNormals[_idx] = _normX;
			a_fNormals[_idx + 1] = _normY;
			a_fNormals[_idx + 2] = _normZ;
			_idx += 3;
			a_fUVCoord[_tIdx] = _s;
			a_fUVCoord[_tIdx + 1] = _t;
			_tIdx += 2;
		}
	}

	_idx = 0;
	for (int i = 0; i < m_fSlices; ++i)
	{
		GLuint _startIndex = i * ((GLuint)m_fStacks + 1);
		GLuint _nextStartIndex = (i + 1) * ((GLuint)m_fStacks + 1);
		for (int j = 0; j < m_fStacks; ++j)
		{
			if (j == 0)
			{
				a_uiElements[_idx] = _startIndex;
				a_uiElements[_idx + 1] = _startIndex + 1;
				a_uiElements[_idx + 2] = _nextStartIndex + 1;
				_idx += 3;
			}
			else if (j == m_fStacks - 1)
			{
				a_uiElements[_idx] = _startIndex + j;
				a_uiElements[_idx + 1] = _startIndex + j + 1;
				a_uiElements[_idx + 2] = _nextStartIndex + j;
				_idx += 3;
			}
			else
			{
				a_uiElements[_idx] = _startIndex + j;
				a_uiElements[_idx + 1] = _startIndex + j + 1;
				a_uiElements[_idx + 2] = _nextStartIndex + j + 1;
				a_uiElements[_idx + 3] = _nextStartIndex + j;
				a_uiElements[_idx + 4] = _startIndex + j;
				a_uiElements[_idx + 5] = _nextStartIndex + j + 1;
				_idx += 6;
			}
		}
	}
}

void Sphere::GenSphere(float a_fRadius, int a_iSlices, int a_iStacks)
{
	unsigned int _indexCount = 0;

	m_fRadius = a_fRadius;
	m_fSlices = (float)a_iSlices;
	m_fStacks = (float)a_iStacks;

	m_uiVertices = (GLuint)((m_fSlices + 1) * (m_fStacks + 1));
	_indexCount = Data.m_uiIndexCount = ((GLuint)m_fSlices * 2 * ((GLuint)m_fStacks - 1)) * 3;

	//vertecies
	float* _vertexData = new float[m_uiVertices * 3];
	float* _normalData = new float[m_uiVertices * 3];
	float* _texData = new float[m_uiVertices * 2];

	unsigned int* _indexData = new unsigned int[_indexCount];

	GenerateVerts(_vertexData, _normalData, _texData, _indexData);

	glGenVertexArrays(1, &Handle.m_uiVAO);
	glBindVertexArray(Handle.m_uiVAO);

	glGenBuffers(1, &Handle.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, Handle.m_uiVBO);

	glBufferData(GL_ARRAY_BUFFER, (3 * m_uiVertices) * sizeof(float), _vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //Pos
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, (3 * m_uiVertices) * sizeof(float), _normalData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); //Norm
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, (2 * m_uiVertices) * sizeof(float), _texData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3); //Tex
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Handle.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned int), _indexData, GL_STATIC_DRAW);

	delete[] _vertexData;
	delete[] _normalData;
	delete[] _texData;
	delete[] _indexData;

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void Sphere::Render() const
{
	glBindVertexArray(Handle.m_uiVAO);
	glDrawElements(GL_TRIANGLES, Data.m_uiIndexCount, GL_UNSIGNED_INT, ((GLubyte*)NULL + (0)));
}

//Torus
Torus::Torus() :Drawable(),
m_iFaces(NULL),
m_iRings(NULL),
m_iSides(NULL)
{

}

Torus::~Torus()
{

}

void Torus::GenerateVerts(float* a_fVerticies, float* a_fNormals, float* a_fUVCoord,
	unsigned int* a_uiElements, float a_fOuterRadius, float a_fInnerRadius)
{
	float _ringFactor = (float)(TWOPI / m_iRings);
	float _sideFactor = (float)(TWOPI / m_iSides);

	int _idx = 0, _tIdx = 0;

	for (int ring = 0; ring < m_iRings; ++ring)
	{
		float _u = ring * _ringFactor;
		float _cu = cosf(_u);
		float _su = sinf(_u);
		for (int side = 0; side < m_iSides; ++side)
		{
			float _v = side * _sideFactor;
			float _cv = cosf(_v);
			float _sv = sinf(_v);
			float _r = (a_fOuterRadius + a_fInnerRadius * _cv);
			a_fVerticies[_idx] = _r * _cu;
			a_fVerticies[_idx + 1] = _r * _su;
			a_fVerticies[_idx + 2] = a_fInnerRadius * _sv;

			a_fNormals[_idx] = _cv * _cu * _r;
			a_fNormals[_idx + 1] = _cv * _su * _r;
			a_fNormals[_idx + 2] = _sv * _r;

			a_fVerticies[_tIdx] = (float)(_u * TWOPI);
			a_fVerticies[_tIdx] = (float)(_v * TWOPI);
			_tIdx += 2;

			float _lenght = sqrtf(a_fNormals[_idx] * a_fNormals[_idx] +
				a_fNormals[_idx + 1] * a_fNormals[_idx + 1] +
				a_fNormals[_idx + 2] * a_fNormals[_idx + 2]);
			a_fNormals[_idx] /= _lenght;
			a_fNormals[_idx + 1] /= _lenght;
			a_fNormals[_idx + 2] /= _lenght;
			_idx += 3;
		}
	}

	_idx = 0;
	for (int ring = 0; ring < m_iRings; ++ring)
	{
		int _ringStart = ring * m_iSides;
		int _nextRingStart = (ring + 1) * m_iSides;
		for (int side = 0; side < m_iSides; ++side)
		{
			int _nextSide = (side + 1) % m_iSides;
			a_uiElements[_idx] = (_ringStart + side);
			a_uiElements[_idx + 1] = (_nextRingStart + side);
			a_uiElements[_idx + 2] = (_nextRingStart + _nextSide);
			a_uiElements[_idx + 3] = (_ringStart + side);
			a_uiElements[_idx + 4] = (_nextRingStart + _nextSide);
			a_uiElements[_idx + 5] = (_ringStart + _nextSide);
			_idx += 6;
		}
	}
}

void Torus::GenTorus(float a_fOuterRadius, float a_fInnerRadius, int a_iNumOfSides, int a_iNumOfRings)
{
	unsigned int _indexCount = 0;

	m_iRings = a_iNumOfRings;
	m_iSides = a_iNumOfSides;

	m_iFaces = m_iRings * m_iSides;
	
	int m_uiVertices = m_iSides * (m_iRings + 1); //One extra ring to duplicate the first ring.

	_indexCount = Data.m_uiIndexCount = 6 * m_iFaces;

	//vertecies
	float* _vertexData = new float[m_uiVertices * 3];
	float* _normalData = new float[m_uiVertices * 3];
	float* _texData = new float[m_uiVertices * 2];

	unsigned int* _indexData = new unsigned int[_indexCount];

	GenerateVerts(_vertexData, _normalData, _texData, _indexData, a_fOuterRadius, a_fInnerRadius);

	glGenVertexArrays(1, &Handle.m_uiVAO);
	glBindVertexArray(Handle.m_uiVAO);

	glGenBuffers(1, &Handle.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, Handle.m_uiVBO);

	glBufferData(GL_ARRAY_BUFFER, (3 * m_uiVertices) * sizeof(float), _vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //Pos
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, (3 * m_uiVertices) * sizeof(float), _normalData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); //Norm
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, (2 * m_uiVertices) * sizeof(float), _texData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3); //Norm
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	glGenBuffers(1, &Handle.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned int), _indexData, GL_STATIC_DRAW);

	delete[] _vertexData;
	delete[] _normalData;
	delete[] _texData;
	delete[] _indexData;

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void Torus::Render() const
{
	glBindVertexArray(Handle.m_uiVAO);
	glDrawElements(GL_TRIANGLES, Data.m_uiIndexCount, GL_UNSIGNED_INT, ((GLubyte*)NULL + (0)));
}

//Gizzle
int Gizzle::m_iID = -1;

Gizzle::Gizzle() : Objects(NONE),
				   m_iActiveObjects(-1),
				   m_iNumOfPlane(NULL),
				   m_iNumOfCube(NULL),
				   m_iNumOfSphere(NULL),
				   m_iNumOfTorus(NULL),
				   m_iNumOfMesh(NULL)
{

}

Gizzle::~Gizzle()
{
	m_moActiveObjects.clear();
}

void Gizzle::AddPlane(vec2 a_vDimensions, glm::ivec2 a_vDivisions, float a_fSMax, float a_fTMax)
{
	Plane* _newPlane = new Plane();

	_newPlane->GenPlane(a_vDimensions, a_vDivisions, a_fSMax, a_fTMax);
	_newPlane->SetID(m_iID);

	m_moActiveObjects.insert(std::make_pair(m_sAlphaID, _newPlane));
}

void Gizzle::AddCube()
{
	Cube* _newCube = new Cube();

	_newCube->GenCube();
	_newCube->SetID(m_iID);

	m_moActiveObjects.insert(std::make_pair(m_sAlphaID, _newCube));
}

void Gizzle::AddSphere(float a_fRadius, int a_iSlices, int a_iStacks)
{
	Sphere* _newSphere = new Sphere();

	_newSphere->GenSphere(a_fRadius, a_iSlices, a_iStacks);
	_newSphere->SetID(m_iID);

	m_moActiveObjects.insert(std::make_pair(m_sAlphaID, _newSphere));
}

void Gizzle::AddTorus(float a_fOuterRadius, float a_fInnerRadius, int a_iNumOfSides, int a_iNumOfRings)
{
	Torus* _newTorus = new Torus();

	_newTorus->GenTorus(a_fOuterRadius, a_fInnerRadius, a_iNumOfSides, a_iNumOfRings);
	_newTorus->SetID(m_iID);

	m_moActiveObjects.insert(std::make_pair(m_sAlphaID, _newTorus));
}

void Gizzle::GenID()
{
	m_iID++;

	switch (Objects)
	{
	case Gizzle::PLANE:
		++m_iNumOfPlane;
		m_sAlphaID = "Plane_" + m_iNumOfPlane;
		++m_iActiveObjects;
		break;
	case Gizzle::CUBE:
		++m_iNumOfCube;
		m_sAlphaID = "Cube_" + m_iNumOfCube;
		++m_iActiveObjects;
		break;
	case Gizzle::SPHERE:
		++m_iNumOfSphere;
		m_sAlphaID = "Sphere_" + m_iNumOfSphere;
		++m_iActiveObjects;
		break;
	case Gizzle::TORUS:
		++m_iNumOfTorus;
		m_sAlphaID = "Torus_" + m_iNumOfTorus;
		++m_iActiveObjects;
		break;
	case Gizzle::MESH:
		++m_iNumOfMesh;
		m_sAlphaID = "Mesh_" + m_iNumOfMesh;
		++m_iActiveObjects;
		break;
	case Gizzle::SKINNED_MESH:
		++m_iNumOfMesh;
		m_sAlphaID = "Mesh_Rig_" + m_iNumOfMesh;
		++m_iActiveObjects;
		break;
	case Gizzle::NONE:
		break;
	default:
		break;
	}
}

void Gizzle::CreateObject(int a_iObjID, float* a_fFloatValues, int* a_iIntValues, bool* a_bOptions, const char* a_pccFileName)
{
	switch (a_iObjID)
	{
	case 0:
		Objects = PLANE;
		break;
	case 1:
		Objects = CUBE;
		break;
	case 2:
		Objects = SPHERE;
		break;
	case 3:
		Objects = TORUS;
		break;
	case 4:
		Objects = MESH;
		break;
	case 5:
		Objects = SKINNED_MESH;
		break;
	default:
		Objects = NONE;
		break;
	}

	GenID();

	switch (Objects)
	{
	case Gizzle::PLANE:
		AddPlane(vec2(a_fFloatValues[0], a_fFloatValues[1]), glm::ivec2(a_iIntValues[0], a_iIntValues[1]), a_fFloatValues[2], a_fFloatValues[3]);
		break;
	case Gizzle::CUBE:
		AddCube();
		break;
	case Gizzle::SPHERE:
		AddSphere(a_fFloatValues[0], a_iIntValues[0], a_iIntValues[1]);
		break;
	case Gizzle::TORUS:
		AddTorus(a_fFloatValues[0], a_fFloatValues[1], a_iIntValues[0], a_iIntValues[1]);
		break;
	case Gizzle::MESH:
		AddMesh(a_pccFileName, a_bOptions[0], a_bOptions[1], a_bOptions[2]);
		break;
	case Gizzle::SKINNED_MESH:
		printf("Skinned mesh func is still underway. please wait \n");
		break;
	case Gizzle::NONE:
		printf("<WARNING>: No object has been selected or the index is out of scope. \n");
		break;
	default:
		break;
	}
}

void Gizzle::Draw()
{
	map<string, Drawable*>::iterator it;
	for (it = m_moActiveObjects.begin(); it != m_moActiveObjects.end(); ++it)
	{
		m_moActiveObjects[it->first]->Render();
	}
}