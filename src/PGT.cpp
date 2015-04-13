#include "PGT.h"
#include <GLFW\glfw3.h>
#include "Camera.h"

//Simplex noise
float SimplexNoise::OctaveNoise2D(const float octaves,
	const float persistance,
	const float scale,
	const float x,
	const float y)
{
	float _result = 0.0f;
	float _total = 0.0f;
	float _freq = scale;
	float _amplitude = 1.0f;

	float _maxAmplitude = 0;

	for (int i = 0; i < octaves; ++i)
	{
		_total += RawNoise2D(x * _freq, y * _freq) * _amplitude;

		_freq *= 2.0f;
		_maxAmplitude += _amplitude;
		_amplitude *= persistance;
	}
	_result = _total / _maxAmplitude;
	return _result;
}

float SimplexNoise::OctaveNoise3D(const float octaves,
	const float persistance,
	const float scale,
	const float x,
	const float y,
	const float z)
{
	float _result = 0.0f;
	float _total = 0.0f;
	float _freq = scale;
	float _amplitude = 1.0f;

	float _maxAmplitude = 0;

	for (int i = 0; i < octaves; ++i)
	{
		_total += RawNoise3D(x * _freq, y * _freq, z * _freq) * _amplitude;

		_freq *= 2.0f;
		_maxAmplitude += _amplitude;
		_amplitude *= persistance;
	}
	_result = _total / _maxAmplitude;
	return _result;
}

float SimplexNoise::ScaledOctaveNoise2D(const float octaves,
	const float persistance,
	const float scale,
	const float loBound,
	const float hiBound,
	const float x,
	const float y)
{
	float _result = OctaveNoise2D(octaves, persistance, scale, x, y) * (hiBound - loBound) / 2 + (hiBound - loBound) / 2;

	return _result;
}

float SimplexNoise::ScaledOctaveNoise3D(const float octaves,
	const float persistance,
	const float scale,
	const float loBound,
	const float hiBound,
	const float x,
	const float y,
	const float z)
{
	float _result = OctaveNoise3D(octaves, persistance, scale, x, y, z) * (hiBound - loBound) / 2 + (hiBound - loBound) / 2;

	return _result;
}

float Dot(const int* grad, const float x, const float y)
{
	float _result = grad[0] * x + grad[1] * y;
	return _result;
}

float SimplexNoise::RawNoise2D(const float xIn, const float yIn)
{
	//Noise contributions from the three corners
	float _n0, _n1, _n2;
	//Skew the input space to detemine which simplex cell we're in
					   //Sqrt of 3.
	float _F2 = 0.5f * (1.414213562f - 1.0f);
	//Hairy factor for 2d
	float _s = (xIn + yIn) * _F2;
	int _i = FastFloor(xIn + _s);
	int _j = FastFloor(yIn + _s);

	float _G2 = (3.0f - 1.414213562f) / 6;
	float _t = (_i + _j) * _G2;
	//Unskew the cell origin back to (x, y) space.
	float _x0 = _i - _t;
	float _y0 = _j - _t;
	//the distance from the origin cell
	float _originX0 = xIn - _x0;
	float _originY0 = yIn - _y0;
	//For the 2d case, the simplex shape is an equlateral triangle.
	//Now we need to determine which simple we are in.
	int _i1, _j1; //Offset for second (middle) corner of the simplex we're in (i, j) coords.
	if (_originX0 > _originY0) //Lower triangle xy corner order. (0, 0)->(1, 0)->(1, 1).
	{
		_i1 = 1;
		_j1 = 0;
	}
	else //Lower triangle xy corner order. (0, 0)->(0, 1)->(1, 1).
	{
		_i1 = 0;
		_j1 = 1;
	}
	//A step of (1, 0) in (i, j) means a step of (1 - c, -c) and a step of
	//(0, 1) in (i, j) means (-c, 1 - c) in (x, y) where c = (3 - sqrt(3)) / 6.
	float _x1 = _originX0 - _i1 + _G2; //Offset for the middle corner in (x, y) Unskewed coord.
	float _y1 = _originY0 - _j1 + _G2;
	float _originX1 = _originX0 - 1.0f + 2.0f * _G2; //Ofset for the last corner in (x, y) Unskewed coord.
	float _originY1 = _originX0 - 1.0f + 2.0f * _G2;
	//Work out the hashed gradient indices of the three simplex corners
	int _ii = _i & 255;
	int _jj = _j & 255;
	int _gi0 = m_iPerm[_ii + m_iPerm[_jj]] % 12;
	int _gi1 = m_iPerm[_ii +_i1 + m_iPerm[_jj + _j1]] % 12;
	int _gi2 = m_iPerm[_ii + 1 + m_iPerm[_jj + 1]] % 12;
	//Calculate the contributions from the three corners
	float _t0 = 0.5f - _x0 * _x0 - _y0 * _y0;
	if (_t0 < 0.0f)
		_n0 = 0.0f;
	else
	{
		_t0 *= _t0;
		_n0 = _t0 * _t0 * Dot(m_iGrad3[_gi0], _x0, _y0);
	}

	float _t1 = 0.5f - _x1 * _x1 - _y1 * _y1;
	if (_t1 < 0.0f)
		_n1 = 0.0f;
	else
	{
		_t1 *= _t1;
		_n1 = _t1 * _t1 * Dot(m_iGrad3[_gi2], _x1, _y1);
	}

	float _t2 = 0.5f - _originX1 * _originX1 - _originY1 * _originY1;
	if (_t2 < 0.0f)
		_n2 = 0.0f;
	else
	{
		_t2 *= _t2;
		_n2 = _t2 * _t2 * Dot(m_iGrad3[_gi1], _originX1, _originY1);
	}

	return 70.0f * (_n0 + _n1 + _n2);
}

float SimplexNoise::RawNoise3D(const float x, const float y, const float z)
{
	return 0;

}

int SimplexNoise::FastFloor(const float x)
{
	int _result = x > 0 ? (int)x : (int)x - 1;
	return _result;
}

float SimplexNoise::MarbleNoiseTexture(const float octaves,
	const float persistance,
	const float scale,
	const float x,
	const float y)
{
	return cosf(x * scale + OctaveNoise2D(octaves, persistance, scale / 3, x, y));
}

//PGT
PGT::PGT()
{

}

PGT::~PGT()
{

}

void PGT::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
	m_oProjection = m_oCamera.GetProjectionTransform(a_vScreenSize.xy());

	LoadShader();

	Perlin.m_fScale = 5;
#define PLANE_SIZE 264
	GeneratePlane(vec2(10, 10), glm::ivec2(PLANE_SIZE, PLANE_SIZE));
	BuildPerlinTex(glm::ivec2(PLANE_SIZE, PLANE_SIZE), 6, 0.03f);
}

void PGT::Update(GLdouble a_dDeltaTime)
{
	m_oView = m_oCamera.GetViewTransform();
	m_oWorld = m_oCamera.GetWorldTransform();

	MoveCamera((float)a_dDeltaTime);
}

void PGT::CleanUpWin()
{
	delete[] Perlin.m_pfData;
	glDeleteProgram(Program.m_uiProgram);
	Application::CleanUpWin();
}

void PGT::Draw()
{
	Use();
	RenderPlane();
}

void PGT::MoveCamera(float a_fDeltaTime)
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
	//Wire mode off
	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Wire mode on
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Culling on
	if (m_bKeys[GLFW_KEY_C])
		glEnable(GL_CULL_FACE);
	//Culling off
	if (m_bKeys[GLFW_KEY_V])
		glDisable(GL_CULL_FACE);
	if (m_bKeys[GLFW_KEY_1])
		Perlin.m_fScale += 0.2f;
	if (m_bKeys[GLFW_KEY_2])
		Perlin.m_fScale -= 0.2f;
}

void PGT::Use()
{
	glUseProgram(Program.m_uiProgram);

	GLint _projUni = glGetUniformLocation(Program.m_uiProgram, "Proj_Trans");
	glUniformMatrix4fv(_projUni, 1, GL_FALSE, value_ptr(m_oProjection));

	GLint _viewUni = glGetUniformLocation(Program.m_uiProgram, "View_Trans");
	glUniformMatrix4fv(_viewUni, 1, GL_FALSE, value_ptr(m_oView));

	GLint _texUni = glGetUniformLocation(Program.m_uiProgram, "Perlin_Tex");
	glUniform1i(_texUni, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Perlin.m_uiTexture);

	GLint _scaleUni = glGetUniformLocation(Program.m_uiProgram, "Scale_Mul");
	glUniform1f(_scaleUni, Perlin.m_fScale);
}

void PGT::LoadShader()
{
	Program.m_uiProgram = m_oShader.GenerateShaderProgram(PERLIN_VERTEX_GLSL, nullptr, PERLIN_FRAGMENT_GLSL);
}

void  PGT::GeneratePlane(vec2 a_vRealDim, glm::ivec2 a_viDim)
{
	//Compute the number of vertices.
	unsigned int _vertexCount = (a_viDim.x + 1) * (a_viDim.y + 1);
	//Allocate vertex data.
	VertexCoord* _vertexData = new VertexCoord[_vertexCount];
	//Compute ideces
	unsigned int _indexCount = a_viDim.x * a_viDim.y * 6;
	unsigned int* _indexData = new unsigned int[_indexCount];
	//Loops
	float _currY = -a_vRealDim.y / 2.0f;
	for (int cl = 0; cl < a_viDim.y + 1; ++cl)
	{
		float _currX = -a_vRealDim.x / 2.0f;
		for (int rw = 0; rw < a_viDim.x + 1; ++rw)
		{
			//Allocate point to grid
			_vertexData[cl * (a_viDim.x + 1) + rw].m_vPos = vec4(_currX, 0.0f, _currY, 1.0f);
			float _x = (float)rw / (float)a_viDim.x;
			float _y = (float)cl / (float)a_viDim.y;
			_vertexData[cl * (a_viDim.x + 1) + rw].m_vUV = vec2(_x, _y);
			_currX += a_vRealDim.x / (float)a_viDim.x;
		}
		_currY += a_vRealDim.y / (float)a_viDim.y;
	}
	//Create indeces
	int _currentIndex = 0;
	for (int cl = 0; cl < a_viDim.y; ++cl)
	{
		for (int rw = 0; rw < a_viDim.x; ++rw)
		{
			_indexData[_currentIndex++] = cl * (a_viDim.x + 1) + rw;
			_indexData[_currentIndex++] = (cl + 1) * (a_viDim.x + 1) + rw;
			_indexData[_currentIndex++] = (cl + 1) * (a_viDim.x + 1) + (rw + 1);

			_indexData[_currentIndex++] = (cl + 1) * (a_viDim.x + 1) + (rw + 1);
			_indexData[_currentIndex++] = cl * (a_viDim.x + 1) + (rw + 1);
			_indexData[_currentIndex++] = cl * (a_viDim.x + 1) + rw;
		}
	}
	m_oPlane.m_uiIndexCount = _indexCount;
	//
	glGenVertexArrays(1, &m_oPlane.m_uiVAO);
	glBindVertexArray(m_oPlane.m_uiVAO);

	glGenBuffers(1, &m_oPlane.m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_oPlane.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexCoord) * _vertexCount, _vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_oPlane.m_uiIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oPlane.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indexCount, _indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexCoord), (void*)sizeof(vec4));
	
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	//free memory
	delete[] _vertexData;
	delete[] _indexData;
}

void PGT::RenderPlane()
{
	glBindVertexArray(m_oPlane.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_oPlane.m_uiIndexCount, GL_UNSIGNED_INT, NULL);
}

void PGT::BuildPerlinTex(glm::ivec2 a_viDim, int a_iOctaves, float a_fPersistance)
{
	float _scale = (1.0f / a_viDim.x) * 3;

	Perlin.m_pfData = new float[a_viDim.x * a_viDim.y];

	for (int cl = 0; cl < a_viDim.y; ++cl)
	{
		for (int rw = 0; rw < a_viDim.x; ++rw)
		{
			float _amplitude = 1.0f;
			float _freq = 1.0f;

			Perlin.m_pfData[cl * a_viDim.x + rw] = 0;

			for (int o = 0; o < a_iOctaves; ++o)
			{
				SimplexNoise _noise;
				float _perlinSample = glm::perlin(vec2(rw, cl) * _scale * _freq) * 0.5f + 0.5f;
					//_noise.OctaveNoise2D(a_iOctaves, a_fPersistance, PLANE_SIZE, rw, cl) * 0.5f + 0.5f;

				_perlinSample *= _amplitude;

				Perlin.m_pfData[cl * a_viDim.x + rw] += _perlinSample;

				_amplitude *= a_fPersistance;
				_freq *= 2.0f;
			}
		}
	}

	glGenTextures(1, &Perlin.m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, Perlin.m_uiTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, a_viDim.x, a_viDim.y, 0, GL_RED, GL_FLOAT, Perlin.m_pfData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

void PGT::GeneratePlane(float a_fXSize, float a_fZSize, int a_iXDivs, int a_iZDivs, float a_fSMax, float a_fTMax)
{
	//m_oPlane.m_uiIndexCount = a_iXDivs * a_iZDivs;

	//float* _v		= new float[3 * (a_iXDivs + 1) * (a_iZDivs + 1)];
	//float* _n		= new float[3 * (a_iXDivs + 1) * (a_iZDivs + 1)];
	//float* _tex		= new float[2 * (a_iXDivs + 1) * (a_iZDivs + 1)];
	//GLuint* _elem	= new GLuint[6 * a_iXDivs * a_iZDivs];

	//float _x2 = a_fXSize / 2.0f;
	//float _z2 = a_fZSize / 2.0f;
	//float _iFact = (float)a_fZSize / a_iZDivs;
	//float _jFact = (float)a_fXSize / a_iXDivs;
	//float _texI = a_fSMax / a_iZDivs;
	//float _texj = a_fTMax / a_iXDivs;

	//float _x, _z;

	//int _vidX = 0, _tidx = 0;

	//for (unsigned int i = 0; i < (unsigned int)a_iZDivs; ++i)
	//{
	//	_z = _iFact * i - _z2;
	//	for (unsigned int j = 0; j < (unsigned int)a_iXDivs; ++j)
	//	{
	//		_x = _jFact * j - _x2;
	//		_v[_vidX]		= _x;
	//		_v[_vidX + 1]	= 0.0f;
	//		_v[_vidX + 2]	= _z;
	//		_n[_vidX]		= 0.0f;
	//		_n[_vidX]		= 1.0f;
	//		_n[_vidX]		= 0.0f;
	//	}
	//}
}