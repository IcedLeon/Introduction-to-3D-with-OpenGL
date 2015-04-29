#include "GraphicsAssignment.h"
#include "TextureLoader.h"
#include "FbxSubLoader.h"

GraphicsAssignment::GraphicsAssignment() : BaseApplication(),
										   //m_uiUBO(NULL),
										   m_uiVBO(NULL),		
										   m_uiNoiseTex(NULL),	
										   m_oTerrainFBO(NULL),
										   m_uiTerrainVBO(NULL),
										   m_uiTerrainIBO(NULL)
{
	UniformTess.InvProj = mat4(1);
	UniformTess.InvView = mat4(1);
	UniformTess.Viewport = vec4(0.0f);
	UniformTess.EyePosWorld = vec4(0.0f);
	for (int i = 0; i < 6; ++i)
		UniformTess.FrustumPlanes[i] = vec4(0.0f);
	UniformTess.LightDir = vec3(0.0f);
	UniformTess.LightDirWorld = vec3(0.0f);
	UniformTess.TileSize = vec3(0.0f);
	UniformTess.GridOrigin = vec3(0.0f);
	UniformTess.Translate = vec3(0.0f);
	UniformTess.Time = NULL;
	UniformTess.TileBoundSphereR = NULL;
	UniformTess.InvFocalLen = NULL;
	UniformTess.InnerTessFactor = NULL;
	UniformTess.OuterTessFactor = NULL;
	UniformTess.NoiseFreq = NULL;
	UniformTess.InvNoiseSize = NULL;
	UniformTess.InvNoise3DSize = NULL;
	UniformTess.HeightScale = NULL;
	UniformTess.TriSize = NULL;
	UniformTess.NoiseOctaves = NULL;
	UniformTess.GridW = NULL;
	UniformTess.GridH = NULL;
	UniformTess.SmoothNormal = false;
	UniformTess.Cull = false;
	UniformTess.LOD = false;
	//Program.m_uiTerrainPipeline = NULL;
}

GraphicsAssignment::~GraphicsAssignment()
{
}

/*Private Method*/
static inline float FloatRand()
{
	return rand() / (float)RAND_MAX;
}

GLuint CreateNoiseTexture2D(int a_iW, int a_iH, GLint a_uiInternalFormat, bool a_bMipmap = false)
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
	   
GLuint CreateNoiseTexture3D(int a_iW, int a_iH, int a_iD, GLint a_uiInternalFormat, bool a_bMipmap = false)
{
	float* _data = new float[a_iW * a_iH * a_iD];
	for (int z = 0; z < a_iD; ++z)
	{
		for (int y = 0; y < a_iH; ++y)
		{
			for (int x = 0; x < a_iW; ++x)
			{
				_data[z * (a_iW * a_iH) + (y * a_iW) + x] = FloatRand();
			}
		}
	}

	GLuint _outTex;
	glGenTextures(1, &_outTex);
	glBindTexture(GL_TEXTURE_3D, _outTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, a_bMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, a_uiInternalFormat, a_iW, a_iH, a_iD, 0, GL_RED, GL_FLOAT, _data);
	if (a_bMipmap) {
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	delete[] _data;
	return _outTex;
}
	   
GLuint CreateNoiseTexture4f3D(int a_iW, int a_iH, int a_iD, GLint a_uiInternalFormat, bool a_bMipmap = false)
{
	float*	_data = new float[a_iW * a_iH * a_iD * 4];
	float* _ptr = _data;
	for (int z = 0; z < a_iD; ++z) 
	{
		for (int y = 0; y < a_iH; ++y) 
		{
			for (int x = 0; x < a_iW; ++x)
			{
				*_ptr++ = FloatRand();
				*_ptr++ = FloatRand();
				*_ptr++ = FloatRand();
				*_ptr++ = FloatRand();
			}
		}
	}

	GLuint _outTex;
	glGenTextures(1, &_outTex);
	glBindTexture(GL_TEXTURE_3D, _outTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, a_bMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, a_uiInternalFormat, a_iW, a_iH, a_iD, 0, GL_RGBA, GL_FLOAT, _data);
	if (a_bMipmap) {
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	delete[] _data;
	return _outTex;
}

void GraphicsAssignment::LoadShaders()
{
#define TERRAIN_VERTEX "./shaders/TerrainVertex.vert"
#define TERRAIN_CONTROL "./shaders/TerrainControl.tcs"
#define TERRAIN_WIRE_VERT "./shaders/TerrainWire.vert"
#define TERRAIN_WIRE_GEOMETRY "./shaders/TerrainWire.geom"
#define TERRAIN_WIRE_FRAG "./shaders/TerrainWire.frag"
#define SKY_VERTEX "./shaders/Skybox.vert"
#define SKY_FRAGMENT "./shaders/Skybox.frag"
#define TERRAIN_TESSELLATION "./shaders/TerrainTessellation.tes"
#define TERRAIN_GEN_VERT "./shaders/TerrainGeneration.vert"
#define TERRAIN_GEN_FRAG "./shaders/TerrainGeneration.frag"
#define TERRAIN_FRAGMENT "./shaders/TerrainFragment.frag"
#define MESH_VERTEX "./shaders/ModelVertex.vert"
#define MESH_FRAGMENT "./shaders/ModelFragment.frag"

	vector<const char*> _shrSource;
	//m_uiTerrainTex1 = TEXLOADER::LoadTexture("./textures/Terrain_1.tga");
	//m_uiTerrainTex2 = TEXLOADER::LoadTexture("./textures/Terrain_2.tga");

	printf("Compiling terrain shader...\n");
	//if (Program.m_oTerrain.GetHandle())
	//	Program.m_oTerrain.Release();
	_shrSource.push_back(TERRAIN_VERTEX);
	_shrSource.push_back(TERRAIN_CONTROL);
	_shrSource.push_back(TERRAIN_TESSELLATION);
	_shrSource.push_back(TERRAIN_FRAGMENT);
	Program.m_oTerrain.CreateProgram(_shrSource);
	_shrSource.clear();
	printf("Compiling wireframe shader...\n");
	//if (Program.m_oTerrainWireFrag.GetHandle())
	//	Program.m_oTerrainWireFrag.Release();
	_shrSource.push_back(TERRAIN_VERTEX);
	_shrSource.push_back(TERRAIN_CONTROL);
	_shrSource.push_back(TERRAIN_TESSELLATION);
	_shrSource.push_back(TERRAIN_WIRE_GEOMETRY);
	_shrSource.push_back(TERRAIN_WIRE_FRAG);
	Program.m_oTerrainWireFrag.CreateProgram(_shrSource);
	_shrSource.clear();

	printf("Compiling Skybox program...\n");
	//if (Program.m_oSkyProg.GetHandle())
	//	Program.m_oSkyProg.Release();
	_shrSource.push_back(SKY_VERTEX);
	_shrSource.push_back(SKY_FRAGMENT);
	Program.m_oSkyProg.CreateProgram(_shrSource);
	_shrSource.clear();

	printf("Compiling terrain generation program...\n");
	//if (Program.m_oGenTerrainProg.GetHandle())
	//	Program.m_oGenTerrainProg.Release();
	_shrSource.push_back(TERRAIN_GEN_VERT);
	_shrSource.push_back(TERRAIN_GEN_FRAG);
	Program.m_oGenTerrainProg.CreateProgram(_shrSource);
	_shrSource.clear();

	printf("Compiling mesh program...\n");
	_shrSource.push_back(MESH_VERTEX);
	_shrSource.push_back(MESH_FRAGMENT);
	Program.m_oMeshProg.CreateProgram(_shrSource);
	Program.m_oMeshProg2.CreateProgram(_shrSource);
	_shrSource.clear();
	/* Assign some extra uniform to the programs */
	/* Note to myself, while assign an uniform doesnt seams like a diffucult task, and it's not,
	the program will fail if the program is not in use (glUseProgram func) you DUMMY!!!. So before initialize any uniform
	remember to activete the desired shader program.!!!*/
	Program.m_oTerrain.Use();
	Program.m_oTerrain.SetUniform("Noise_Tex", 0);
	Program.m_oTerrain.Disable();

	Program.m_oGenTerrainProg.Use();
	Program.m_oGenTerrainProg.SetUniform("Noise_Tex", 0);
	Program.m_oGenTerrainProg.Disable();

	Program.m_oSkyProg.Use();
	Program.m_oSkyProg.SetUniform("Rand_Tex3D", 0);
	Program.m_oSkyProg.Disable();

	Program.m_oMeshProg.Use();
	Program.m_oMeshProg.SetUniform("Diffuse_Tex", 0);
	Program.m_oMeshProg.SetUniform("Normal_Tex", 1);
	Program.m_oMeshProg.SetUniform("Specular_Tex", 2);
	Program.m_oMeshProg.Disable();

	Program.m_oMeshProg2.Use();
	Program.m_oMeshProg2.SetUniform("Diffuse_Tex", 0);
	Program.m_oMeshProg2.SetUniform("Normal_Tex", 1);
	Program.m_oMeshProg2.SetUniform("Specular_Tex", 2);
	Program.m_oMeshProg2.Disable();
}

void GraphicsAssignment::InitTerrain()
{
	if (m_oTerrainFBO)
	{
		delete m_oTerrainFBO;
	}

	SimpleFBO::Descrip _fboDescript;
	_fboDescript.m_uiW = UniformTess.GridW * 64;
	_fboDescript.m_uiH = UniformTess.GridH * 64;
	_fboDescript.m_oColour.m_eFormat = GL_RGBA;
	_fboDescript.m_oColour.m_eType = GL_FLOAT;
	_fboDescript.m_oColour.m_iFilter = GL_LINEAR;

	m_oTerrainFBO = new SimpleFBO();
	m_oTerrainFBO->CreateFBO(_fboDescript);
}

void GraphicsAssignment::InitOneTimeUniform()
{
	/* Inner tessellation factor */
	UniformTess.InnerTessFactor = 32.0f;
	/* Outer tessellation factor */
	UniformTess.OuterTessFactor = 32.0f;
	/* Noise frequency */
	UniformTess.NoiseFreq		= 0.25f;
	/* Height scale */
	UniformTess.HeightScale		= 2.5f;
	/* Octaves */
	UniformTess.NoiseOctaves	= 18;
	/* Triangle size */
	UniformTess.TriSize			= 5.0f;
	/* Culling */
	UniformTess.Cull			= false;
	/* Level of detail */
	UniformTess.LOD				= true;
	/* Grid origin */
	UniformTess.GridOrigin		= vec3(-28.0f, 0.0f, -28.0f);
	/* Grid width */
	UniformTess.GridW			= 84;
	/* Grid Height */
	UniformTess.GridH			= 84;
	/* Tile size */
	UniformTess.TileSize		= vec3(1.0f, 0.0f, 1.0f);
	/* Half a tile */
	vec3 _halfTileSize			= vec3(UniformTess.TileSize.x, UniformTess.HeightScale, UniformTess.TileSize.z) * 0.5;
	UniformTess.TileBoundSphereR = glm::length(_halfTileSize);
	/* Light direction */
	m_vLightDirection			= vec3(-1.0f, -0.25f, 1.0f);
	m_vLightDirection			= normalize(m_vLightDirection);
	/* Projection */
	DATA.TRANSFORM.m_mProjection = DATA.m_oCurrCamera->GetProjectionTransform(APPINFO.m_viWinSize.xy(), 1.0f, 10000.0f);
	UniformTess.Projection		 = DATA.TRANSFORM.m_mProjection;
}

void GraphicsAssignment::SettingProgramsUniform()
{
	Program.m_oTerrain.Use();
	SetTerrainUniform();
	Program.m_oTerrain.Disable();

	Program.m_oTerrainWireFrag.Use();
	SetWireframeUniform();
	Program.m_oTerrainWireFrag.Disable();

	Program.m_oSkyProg.Use();
	SetSkyUniform();
	Program.m_oSkyProg.Disable();

	Program.m_oGenTerrainProg.Use();
	SetGenerationUniform();
	Program.m_oGenTerrainProg.Disable();

	Program.m_oMeshProg.Use();
	SetMeshUniform();
	Program.m_oMeshProg.Disable();

	Program.m_oMeshProg2.Use();
	SetMeshUniform();
	Program.m_oMeshProg2.Disable();
}

void GraphicsAssignment::SetSkyUniform()
{
	const char* _uniformNames[] =
	{
		"InvProj", "InvView",
		"InvNoise3DSize", "Time",
		"LightDirWorld", "EyePosWorld", 
		"Translate"
	};

	for (unsigned int i = 0; i < 7; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.InvProj);
			break;
		case 1:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.InvView);
			break;
		case 2:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.InvNoise3DSize);
			break;
		case 3:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.Time);
			break;
		case 4:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.LightDirWorld);
			break;
		case 5:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.EyePosWorld);
			break;
		case 6:
			Program.m_oSkyProg.SetUniform(_uniformNames[i], UniformTess.Translate);
			break;
		default:
			printf("<WARNING>: No sky default uniform.");
			break;
		}
	}
}

void GraphicsAssignment::SetTerrainUniform()
{
	const char* _uniformNames[] =
	{
		/*Mat4*/
		"Projection", 
		"View", "VP",
		/*vec4*/
		"Viewport", "FrustumPlanes", "EyePosWorld",
		/*vec3*/
		"GridOrigin", "TileSize", "LightDirWorld",
		/*vec2*/
		"Translate",
		/*float*/
		"TriSize", "HeightScale", "TileBoundSphereR",
		"InnerTessFactor", "OuterTessFactor", "InvNoiseSize",
		"NoiseFreq",
		/*int*/
		"GridW", "GridH", "NoiseOctaves",
		/*bool*/
		"Cull", "LOD"
	};

	for (unsigned int i = 0; i < 22; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.Projection);
			break;
		case 1:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.View);
			break;
		case 2:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.VP);
			break;
		case 3:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.Viewport);
			break;
		case 4:
			Program.m_oTerrain.SetUniform(_uniformNames[i], *UniformTess.FrustumPlanes, 6);
			break;
		case 5:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.EyePosWorld);
			break;
		case 6:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.GridOrigin);
			break;
		case 7:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.TileSize);
			break;
		case 8:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.LightDirWorld);
			break;
		case 9:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.Translate);
			break;
		case 10:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.TriSize);
			break;
		case 11:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.HeightScale);
			break;
		case 12:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.TileBoundSphereR);
			break;
		case 13:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.InnerTessFactor);
			break;
		case 14:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.OuterTessFactor);
			break;
		case 15:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.InvNoiseSize);
			break;
		case 16:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.NoiseFreq);
			break;
		case 17:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.GridW);
			break;
		case 18:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.GridH);
			break;
		case 19:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.NoiseOctaves);
			break;
		case 20:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.Cull);
			break;
		case 21:
			Program.m_oTerrain.SetUniform(_uniformNames[i], UniformTess.LOD);
			break;
		default:
			break;
		}
	}
}

void GraphicsAssignment::SetGenerationUniform()
{
	const char* _uniformNames[] =
	{
		/*vec3*/
		"GridOrigin", "TileSize",
		/*float*/
		"HeightScale", "InvNoiseSize",
		"NoiseFreq",
		/*int*/
		"GridW", "GridH", "NoiseOctaves"
	};

	for (unsigned int i = 0; i < 8; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.GridOrigin);
			break;
		case 1:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.TileSize);
			break;
		case 2:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.HeightScale);
			break;
		case 3:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.InvNoiseSize);
			break;
		case 4:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.NoiseFreq);
			break;
		case 5:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.GridW);
			break;
		case 6:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.GridH);
			break;
		case 7:
			Program.m_oGenTerrainProg.SetUniform(_uniformNames[i], UniformTess.NoiseOctaves);
			break;
		default:
			break;
		}
	}
}

void GraphicsAssignment::SetWireframeUniform()
{
	const char* _uniformNames[] =
	{
		/*vec3*/
		"GridOrigin", "TileSize",
		/*int*/
		"GridW", "GridH",
	};

	for (unsigned int i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oTerrainWireFrag.SetUniform(_uniformNames[i], UniformTess.GridOrigin);
			break;
		case 1:
			Program.m_oTerrainWireFrag.SetUniform(_uniformNames[i], UniformTess.TileSize);
			break;
		case 2:
			Program.m_oTerrainWireFrag.SetUniform(_uniformNames[i], UniformTess.GridW);
			break;
		case 3:
			Program.m_oTerrainWireFrag.SetUniform(_uniformNames[i], UniformTess.GridH);
			break;
		default:
			break;
		}
	}
}

void GraphicsAssignment::SetMeshUniform()
{
	const char* _uniformNames[] =
	{
		/* Mat4*/
		"VP",
		/* Vec4 */
		"EyePosWorld",
		/* Vec3 */
		"LightDirWorld"
	};

	for (unsigned int i = 0; i < 3; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oMeshProg.SetUniform(_uniformNames[i], UniformTess.VP);
			break;
		case 1:
			Program.m_oMeshProg.SetUniform(_uniformNames[i], UniformTess.EyePosWorld);
			break;
		case 2:
			Program.m_oMeshProg.SetUniform(_uniformNames[i], UniformTess.LightDirWorld);
			break;
		default:
			break;
		}
	}
}

void GraphicsAssignment::SetMeshUniform2()
{
	const char* _uniformNames[] =
	{
		/* Mat4*/
		"VP",
		/* Vec4 */
		"EyePosWorld",
		/* Vec3 */
		"LightDirWorld"
	};

	for (unsigned int i = 0; i < 3; ++i)
	{
		switch (i)
		{
		case 0:
			Program.m_oMeshProg2.SetUniform(_uniformNames[i], UniformTess.VP);
			break;
		case 1:
			Program.m_oMeshProg2.SetUniform(_uniformNames[i], UniformTess.EyePosWorld);
			break;
		case 2:
			Program.m_oMeshProg2.SetUniform(_uniformNames[i], UniformTess.LightDirWorld);
			break;
		default:
			break;
		}
	}
}

void GraphicsAssignment::InitRendering(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	LoadShaders();
	//With a single default program pipeline I should be able to andle 
	//Binding and unbingind of the separete shaders object with less call,
	//Since this demo utilise quite few programs.
	/*glGenProgramPipelines(1, &Program.m_uiTerrainPipeline);
	glBindProgramPipeline(Program.m_uiTerrainPipeline);*/
	//glGenBuffers(1, &m_uiUBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, m_uiUBO);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformTess), &UniformTess, GL_STREAM_DRAW);

	float _vertexData[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glGenBuffers(1, &m_uiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertexData), _vertexData, GL_STATIC_DRAW);

	InitOneTimeUniform();

	srand(0);
	int _noiseSize = 256;
	int _noise3DSize = 64;
	/* 2D noise texture */
	m_uiNoiseTex = CreateNoiseTexture2D(_noiseSize, _noiseSize, GL_R16F);
	/* 3D noise texture */
	m_uiNoise3DTex = CreateNoiseTexture4f3D(_noise3DSize, _noise3DSize, _noise3DSize, GL_RGBA16F);
	/* Setting the inverse of the noise to pass into the UBO later */
	UniformTess.InvNoiseSize = 1.0f / _noiseSize;
	UniformTess.InvNoise3DSize = 1.0f / _noise3DSize;

	SettingProgramsUniform();

	glGenQueries(1, &m_uiGPUQuery);
	
	InitTerrain();
}

void DrawSimpleQuad(GLint a_iPosAttrib, GLint a_iTexCoordAttrib)
{
	// vertex positions in NDC tex-coords
	static const float _fullScreenQuadData[] =
	{
		-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f
	};

	glVertexAttribPointer(a_iPosAttrib, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), _fullScreenQuadData);
	glVertexAttribPointer(a_iTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), &_fullScreenQuadData[4]);

	glEnableVertexAttribArray(a_iPosAttrib);
	glEnableVertexAttribArray(a_iTexCoordAttrib);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(a_iPosAttrib);
	glDisableVertexAttribArray(a_iTexCoordAttrib);
}

void GraphicsAssignment::UpdateTerrainTex()
{
	GLuint _prevFBO = NULL;

	glGetIntegerv(0x8CA6, (GLint*)&_prevFBO);

	m_oTerrainFBO->Bind();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	//Bind again the UBO to the updated state of the variables from the CPU side.
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uiUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uiUBO); 
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformTess), &UniformTess);

	Program.m_oGenTerrainProg.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiNoiseTex);

	DrawSimpleQuad(0, 8);

	Program.m_oGenTerrainProg.Disable();

	//Restore previous fbo
	glBindFramebuffer(GL_FRAMEBUFFER, _prevFBO);

	glViewport(0, 0, APPINFO.m_viWinSize.x, APPINFO.m_viWinSize.y);

	printf("Generated terrain texture %d x %d.\n", m_oTerrainFBO->m_uiWidth, m_oTerrainFBO->m_uiHeight);
}

//Extraction of the frustim planes
//Equation ref: Mathemathics for 3D game programming and computer graphics
//Page 119, parag (5.60)
void GraphicsAssignment::FrustumPlanes(mat4& a_mView, mat4& a_mProj, vec4* a_vPlane)
{
	mat4 _viewProj = a_mProj * a_mView;
	//Grabbing them from camera space view.
	a_vPlane[0] = _viewProj[3] + _viewProj[2];   // near
	a_vPlane[1] = _viewProj[3] - _viewProj[2];   // far
	a_vPlane[2] = _viewProj[3] + _viewProj[0];   // left
	a_vPlane[3] = _viewProj[3] - _viewProj[0];   // right
	a_vPlane[4] = _viewProj[3] + _viewProj[1];   // bottom
	a_vPlane[5] = _viewProj[3] - _viewProj[1];   // top

	// normalize planes
	for (int i = 0; i < 6; ++i) 
	{
		//float l = length(a_vPlane[i]);
		a_vPlane[i] = normalize(a_vPlane[i]);// / l;
	}
}

bool GraphicsAssignment::SphereIsFrosted(vec3 a_vPos, float a_fRay, vec4* a_vPlane)
{
	vec4 hp = vec4(a_vPos, 1.0f);

	for (int i = 0; i < 6; ++i) 
	{
		if (dot(hp, a_vPlane[i]) + a_fRay < 0.0f) 
		{
			// sphere outside plane
			return false;
		}
	}
	return true;
}

void GraphicsAssignment::DrawTerrain()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//glBindProgramPipeline(Program.m_uiTerrainPipeline);
	//
	//glUseProgramStages(Program.m_uiTerrainPipeline, GL_VERTEX_SHADER_BIT, Program.m_oTerrainVertex.GetHandle());
	//
	//glUseProgramStages(Program.m_uiTerrainPipeline, GL_TESS_CONTROL_SHADER_BIT, Program.m_oTerrainTessControl.GetHandle());
	//
	//glUseProgramStages(Program.m_uiTerrainPipeline, GL_TESS_EVALUATION_SHADER_BIT, Program.m_oTerrainEvaluation.GetHandle());
	//Program.m_oTerrain.Use();

	Program.m_oTerrain.Use();
	//if (m_bWireFrame)
	//{
	//	//glUseProgramStages(Program.m_uiTerrainPipeline, GL_GEOMETRY_SHADER_BIT, Program.m_oTerrainWireGeometry.GetHandle());
		//
		//glUseProgramStages(Program.m_uiTerrainPipeline, GL_FRAGMENT_SHADER_BIT, Program.m_oTerrainWireFrag.GetHandle());
	//	Program.m_oTerrainWireFrag.Use();
	//}
	//else
	//{
	//	Program.m_oTerrainWireFrag.Disable();	
	//	//glUseProgramStages(Program.m_uiTerrainPipeline, GL_GEOMETRY_SHADER_BIT, NULL);
		//
		//glUseProgramStages(Program.m_uiTerrainPipeline, GL_FRAGMENT_SHADER_BIT, Program.m_oTerrainFragment.GetHandle());
	//}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiNoiseTex);

	glPatchParameteri(GL_PATCH_VERTICES, 1);

	int _instances = UniformTess.GridW * UniformTess.GridH;
	glDrawArraysInstanced(GL_PATCHES, 0, 1, _instances);

	Program.m_oTerrain.Disable();
	glBindTexture(GL_TEXTURE_2D, NULL);
	//glBindProgramPipeline(NULL);
	glDisable(GL_CULL_FACE);
}

void GraphicsAssignment::DrawQuad(float a_fZ)
{
	GLfloat _vertexData[] = 
	{
		-1.0f, -1.0, a_fZ,
		 1.0f, -1.0, a_fZ,
		-1.0f,  1.0, a_fZ,
		 1.0f,  1.0, a_fZ,
	};

	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, _vertexData);

	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
}

void GraphicsAssignment::DrawSky()
{
	glEnable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_uiNoise3DTex);

	Program.m_oSkyProg.Use();
	DrawQuad(0.9999f);
	Program.m_oSkyProg.Disable();

	glBindTexture(GL_TEXTURE_3D, NULL);
}

void GraphicsAssignment::InitBars()
{
#define MAIN_BAR "Terrain"
	TwInit(TW_OPENGL, NULL);
	m_oBar = TwNewBar(MAIN_BAR);
	float _test = 13.0f;
	TwAddVarRW(m_oBar, "LightDirWorld", TW_TYPE_FLOAT, &_test, "label='LOD'");
	TwAddVarRW(m_oBar, "LightDirWorld2", TW_TYPE_FLOAT, &_test, "label='LOD'");
	TwAddVarRW(m_oBar, "LightDirWorld3", TW_TYPE_FLOAT, &_test, "label='LOD'");
	TwAddVarRW(m_oBar, "LightDirWorld4", TW_TYPE_FLOAT, &_test, "label='LOD'");
	TwAddVarRW(m_oBar, "LightDirWorld5", TW_TYPE_FLOAT, &_test, "label='LOD'");
}

/*Public Method*/
void GraphicsAssignment::Init(BaseApplication* a_oCurrApp, vec3 a_vCamPos, ivec2 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	App::BaseApplication::Init(a_oCurrApp, a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
	/* Init related function for the Rendering */
	InitRendering();
	/* Loading models */
	m_oFbxLoader = new FbxSubLoader();

	m_oFbxLoader2 = new FbxSubLoader();

	const char* _fileNames[] = 
	{
		"./models/rigged/Pyro/pyro.fbx",
		"./models/rigged/Enemytank/EnemyTank.fbx"
	};

	m_oFbxLoader->LoadFileFromSrc(_fileNames[0]);

	m_oFbxLoader2->LoadFileFromSrc(_fileNames[1]);

	InitBars();
}

void GraphicsAssignment::Render()
{
	glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	/* UBO Binding */
	//glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uiUBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, m_uiUBO);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformTess), &UniformTess);

	/* Query number of primitives */
	glBeginQuery(GL_PRIMITIVES_GENERATED, m_uiGPUQuery);
	/* Draw terrain */
	DrawTerrain();
	/* End Query */
	glEndQuery(GL_PRIMITIVES_GENERATED);
	/* Skybox */
	DrawSky();
	glGetQueryObjectuiv(m_uiGPUQuery, GL_QUERY_RESULT, &m_uiNumOfPrim);

	m_oFbxLoader->Draw(Program.m_oMeshProg, glm::translate(vec3(10.0, 220.0, 70.0)));

	m_oFbxLoader2->Draw(Program.m_oMeshProg2, glm::translate(vec3(210.0, 220.0, 170.0)));

	TwDraw();

	//Print number of primitives
	//UpdateTerrainTex();
}

void GraphicsAssignment::Update(float a_fDeltaT)
{
	//glBindProgramPipeline(NULL);
	/* Update the Uniform struct (UBO) */
	/* Projection */
	UniformTess.Projection = DATA.TRANSFORM.m_mProjection;
	/* View */
	DATA.TRANSFORM.m_mView = DATA.m_oCurrCamera->GetViewTransform();
	/* Model */
	DATA.TRANSFORM.m_mWorld = DATA.m_oCurrCamera->GetWorldTransform();
	/* ModelView */
	UniformTess.View = DATA.TRANSFORM.m_mView;
	/* MVP */
	UniformTess.VP = DATA.TRANSFORM.m_mProjection * DATA.TRANSFORM.m_mView;
	/* Inverse Projection */
	UniformTess.InvProj = glm::inverse(DATA.TRANSFORM.m_mProjection);
	/* Inverse View */
	UniformTess.InvView = DATA.m_oCurrCamera->GetWorldTransform();
	/* Frustum planes */
	FrustumPlanes(UniformTess.View, DATA.TRANSFORM.m_mProjection, UniformTess.FrustumPlanes);
	/* Restore viewport */
	glViewport(0, 0, APPINFO.m_viWinSize.x, APPINFO.m_viWinSize.y);
	/* Viewport */
	UniformTess.Viewport = vec4(0, 0, APPINFO.m_viWinSize.x, APPINFO.m_viWinSize.y);
	/* Eye positions */
	UniformTess.EyePosWorld = UniformTess.InvView * vec4(0.0f, 0.0f, 0.0f, 1.0f); //<--Simply transform it to vector.
	/* Light direction world */
	UniformTess.LightDirWorld = m_vLightDirection;
	/* Light direction */
	UniformTess.LightDir = vec3(UniformTess.View * vec4(normalize(m_vLightDirection), 0.0)); //To eye space
	/* Time*/
	UniformTess.Time = (float)DATA.TIME.m_dDeltaT;
	//UpdateTerrainTex();
	SettingProgramsUniform();
	/* Key updater */
	UpdateInput();
	/* Mesh updater */
	m_oFbxLoader->UpdateMesh(Program.m_oMeshProg, a_fDeltaT);
	m_oFbxLoader2->UpdateMesh(Program.m_oMeshProg2, a_fDeltaT);
}

void GraphicsAssignment::UpdateInput()
{
	/* Camera controls */
	if (m_bKeys[GLFW_KEY_W])
	{
		DATA.m_oCurrCamera->KeyboardInput(FORWARD, (float)DATA.TIME.m_dDeltaT);
		UniformTess.Translate.z -= (float)DATA.TIME.m_dDeltaT * 2.0f;
	}
	if (m_bKeys[GLFW_KEY_S])
	{
		DATA.m_oCurrCamera->KeyboardInput(BACKWARD, (float)DATA.TIME.m_dDeltaT);
		UniformTess.Translate.z += (float)DATA.TIME.m_dDeltaT * 2.0f;
	}
	if (m_bKeys[GLFW_KEY_A])
	{
		DATA.m_oCurrCamera->KeyboardInput(LEFT, (float)DATA.TIME.m_dDeltaT);
		UniformTess.Translate.x -= (float)DATA.TIME.m_dDeltaT * 2.0f;
	}
	if (m_bKeys[GLFW_KEY_D])
	{
		DATA.m_oCurrCamera->KeyboardInput(RIGHT, (float)DATA.TIME.m_dDeltaT);
		UniformTess.Translate.x += (float)DATA.TIME.m_dDeltaT * 2.0f;
	}
	if (m_bKeys[GLFW_KEY_R])
	{
		LoadShaders();
	}
	///* Wire mode off */
	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/* Wire mode on */
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GraphicsAssignment::OnKey(GLint a_iKey, GLint a_iAction)
{
	TwEventKeyGLFW(a_iKey, a_iAction);
}

void GraphicsAssignment::OnMouseButton(GLint a_iButton, GLint a_iAction)
{
	TwEventMouseButtonGLFW(a_iButton, a_iAction);
}

void GraphicsAssignment::OnMouseMove(GLdouble a_dMouseX, GLdouble a_dMouseY)
{
	TwEventMousePosGLFW((int)a_dMouseX, (int)a_dMouseY);
}

void GraphicsAssignment::OnMouseWheel(GLdouble a_dPosition)
{
	TwEventMouseWheelGLFW((int)a_dPosition);
}

void GraphicsAssignment::Shutdown()
{
	delete m_oTerrainFBO;
	delete m_oFbxLoader;
	delete m_oFbxLoader2;
}