#include "FurSimApp.h"
#include "TextureLoader.h"

#define LOG(_log, ...) { printf(_log, __VA_ARGS__); }

FurSim::FurSim()
{

}

FurSim::~FurSim()
{

}

static inline float FloatRand()
{
	return rand() / (float)RAND_MAX;
}

GLuint CreateNoiseText2D(int a_iW, int a_iH, GLint a_uiInternalFormat, bool a_bMipmap = false)
{
	float* _data = new float[a_iW * a_iH];

	for (int y = 0; y < a_iH; ++y)
	{
		for (int x = 0; x < a_iW; ++x)
		{
			_data[y * a_iW + x] = FloatRand();
		}
	}

	GLuint _outTex = NULL;
	glGenTextures(1, &_outTex);
	glBindTexture(GL_TEXTURE_2D, _outTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_bMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, a_uiInternalFormat, a_iW, a_iH, 0, GL_RED, GL_FLOAT, _data);

	if (a_bMipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	delete[] _data;
	return _outTex;
}

bool TexCoordShape(MeshData& a_oShape)
{
	return false;
}

//void FurSim::InitUniform()
//{
//	m_oProgram.Use();
//	const char* _uniformNames[] =
//	{
//		"u_projectionMatrix",
//		"u_modelViewMatrix",
//		"u_normalMatrix",
//		"u_light.direction",
//		"u_light.ambientColor",
//		"u_light.diffuseColor",
//		"u_light.specularColor",
//		"u_textureFurColor",
//		"a_vertex",
//		"a_normal",
//		"a_texCoord"
//	};
//
//	for (unsigned int i = 0; i < 11; ++i)
//	{
//		switch (i)
//		{
//		case 0:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 1:
//			//m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m);
//			break;
//		case 2:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 3:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 4:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 5:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 6:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 7:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 8:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 9:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		case 10:
//			m_oProgram.SetUniform(_uniformNames[i], DATA.TRANSFORM.m_mProjection);
//			break;
//		}
//	}
//}

void FurSim::Render()
{

}

void FurSim::Update(float a_fDeltaT)
{

}

void FurSim::OnResize(GLint a_iWidth, GLint a_iHeight)
{

}

void FurSim::OnKey(GLint a_iKey, GLint a_iAction)
{

}

void FurSim::OnMouseButton(GLint a_iButton, GLint a_iAction)
{

}

void FurSim::OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY)
{

}

void FurSim::OnMouseWheel(GLdouble a_dPosition)
{

}

void FurSim::Shutdown()
{

}

void FurSim::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	m_oMyLights = { { vec3(1.0f, 1.0f, 1.0f) }, { vec4(0.3f, 0.3f, 0.3f, 1.0f) }, { vec4(1.0f) }, { vec4(1.0f) } };

	DATA.TRANSFORM.m_mProjection = DATA.m_oCurrCamera->GetProjectionTransform(APPINFO.m_viWinSize.xy(), 1.0f, 10000.0f);

	m_uiTextureFurColour = TEXLOADER::LoadTexture("./textures/tiger.tga", false, true);

	int _noiseSize = 64;
	m_uiTexFurStrength = CreateNoiseText2D(_noiseSize, _noiseSize, GL_R16F, true);


}

void FurSim::LoadShaders()
{
	vector<const char*> _shrSource;

	LOG("Compiling ambient diffuse texture shaders...");
	_shrSource.push_back(AMBIENT_VERT);
	_shrSource.push_back(AMBIENT_FRAG);
	m_oProgram.CreateProgram(_shrSource);
	_shrSource.clear();

	LOG("Compiling fur simulation shaders...");
	_shrSource.push_back(FUR_VERTEX);
	_shrSource.push_back(FUR_GEOM);
	_shrSource.push_back(FUR_FRAG);
	m_oFurProgram.CreateProgram(_shrSource);
	_shrSource.clear();
}

MeshData* FurSim::LoadMesh(FBXFile* a_oFile)
{
	MeshData* _result = new MeshData();

	unsigned int _meshCount = a_oFile->getMeshCount();

	for (unsigned int meshIndx = 0; meshIndx < _meshCount; ++meshIndx)
	{
		FBXMeshNode* _currMesh = a_oFile->getMeshByIndex(meshIndx);

		_result->m_uiVertexCount = _currMesh->m_vertices.size();
		_result->m_uiIndexCount  = _currMesh->m_indices.size();

		glGenBuffers(1, &_result->m_uiVBO);
		glGenBuffers(1, &_result->m_uiIBO);
		glGenVertexArrays(1, &_result->m_uiVAO);

		glBindVertexArray(_result->m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, _result->m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* _currMesh->m_vertices.size(),
			_currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _result->m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* _currMesh->m_indices.size(),
			_currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //Position
		glEnableVertexAttribArray(1); //Normal

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
	return _result;
}