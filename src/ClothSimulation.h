#ifndef _CLOTHSIMULATION_H_
#define _CLOTHSIMULATION_H_
//Lib
#include "Application.h"
#include "ShaderCompiler.h"
#include <fstream>
//Using macro
using std::ifstream;
using std::ofstream;
using glm::ivec2;
//Macro
#define ADS_VERTEX				"./shaders/AdsVertex.glsl"
#define ADS_FRAGMENT			"./shaders/AdsFragment.glsl"
#define CLOTH_COMPUTE			"./shaders/ClothCompute.glsl"
#define CLOTH_NORMAL_COMPUTE	"./shaders/ClothNormalCompute.glsl"

class TGA_IO
{
private:
	class IOException : public std::runtime_error
	{
	public:
		IOException(const std::string& a_sMSG) : std::runtime_error(a_sMSG) { }
	};
	int ReadShort(ifstream & stream)
	{
		unsigned char ic;     // input char
		int result;           // result

		ic		= stream.get();
		result	= ic;
		ic		= stream.get();
		result |= ((unsigned int)ic << 8);
		return result;
	}
	GLubyte* ReadFile(const char* a_pccFileName, int& a_iWidht, int& a_iHeight)
	{
		// Open file for reading
		std::ifstream _inFile(a_pccFileName, std::ios::binary);

		try {
			if (!_inFile) {
				std::string msg = std::string("Error: can't open ") + a_pccFileName;
				throw IOException(msg);
			}

			int idLen			= _inFile.get();	  // Length of image ID field
			int mapType			= _inFile.get();	  // Color map type (expect 0 == no color map)
			int typeCode		= _inFile.get();	  // Image type code (expect 2 == uncompressed)
			_inFile.ignore(5);						  // Color map info (ignored)
			int xOrigin			= ReadShort(_inFile); // X origin
			int yOrigin			= ReadShort(_inFile); // Y origin
			a_iWidht			= ReadShort(_inFile); // Image width
			a_iHeight			= ReadShort(_inFile); // Image height
			int bpp				= _inFile.get();      // Bits per pixel (expect 24 or 32)
			_inFile.ignore();						  // Image descriptor (expect 0 for 24bpp and 8 for 32bpp)

			if (typeCode != 2 || mapType != 0) {
				throw IOException("File does not appear to be a non-color-mapped, uncompressed TGA image");
			}

			if (bpp != 24 && bpp != 32) {
				throw IOException("File must be 24 or 32 bits per pixel.\n");
			}

			// Skip the image ID data
			if (idLen > 0) _inFile.ignore(idLen);

			// Color map data would be here, but we assume no color map

			printf("%s: (%d x %d) %d bpp origin (%d, %d)\n", a_pccFileName, a_iWidht, a_iHeight, bpp,
				xOrigin, yOrigin);

			// Read pixel data
			GLubyte* _p = new GLubyte[a_iWidht * a_iHeight * 4];
			// 24 bpp -- Blue, Green, Red
			// 32 bpp -- Blue, Green, Red, Alpha
			// p -- stored as RGBA
			for (unsigned int i = 0; i < (unsigned int)(a_iWidht * a_iHeight); i++)
			{
				_p[i * 4 + 2]	= _inFile.get();  // Blue
				_p[i * 4 + 1]	= _inFile.get();  // Green
				_p[i * 4]		= _inFile.get();  // Red 
				if (bpp == 32)
					_p[i * 4 + 3] = _inFile.get();
				else
					_p[i * 4 + 3] = 0xFF;
			}

			_inFile.close();
			return _p;
		}
		catch (IOException & e) {
			_inFile.close();
			throw e;
		}
	}
	GLuint LoadTexture(const char* a_pccFileName, GLint& a_iWidht, GLint& a_iHeight)
	{
		GLubyte * _data = ReadFile(a_pccFileName, a_iWidht, a_iHeight);

		GLuint _texID;
		glGenTextures(1, &_texID);

		glBindTexture(GL_TEXTURE_2D, _texID);

		// Allocate storage
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, a_iWidht, a_iHeight);
		// Copy data into storage
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, a_iWidht, a_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, _data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete[] _data;

		return _texID;
	}
public:
	GLuint GetTexture(const char* a_pccFileName)
	{
		GLint _w, _h;
		return LoadTexture(a_pccFileName, _w, _h);
	}
};

class ClothSimulation : public Application
{
private:
	ShaderCompiler	m_oShader;
	TGA_IO			m_oTexLoader;
	//
	vec2			m_vClothSize;
	//
	ivec2			m_viParticleNum;
	//
	float			m_fSpeed;
	//
	GLuint			m_uiRenderProgram,
					m_uiComputeProgram,
					m_uiComputeNormProgram;
	//
	GLuint			m_uiReadBuff,
					m_uiNormBuff,
					m_uiElBuff,
					m_uiTCBuff,
					m_uiPosBuff[2],
					m_uiVelBuff[2];
	//
	struct
	{
		GLuint		m_uiVAO;
		GLuint		m_uiIndices;
	} ClothData;

	struct
	{
		GLint		m_iLightPos,
					m_iLightIntensity,
					m_iDiffuseReflectivity,
					m_iAmbientReflectivity,
					m_iSpecularReflectivity,
					m_iSpecularShininessFac;
	} RenderUni;

	struct
	{
		GLint		m_iRestLenghtHoriz,
					m_iRestLenghtVert,
					m_iRestLenghtDiag;
	} ComputeUni;

public:
	ClothSimulation();
	~ClothSimulation();

	virtual void InitWindow(vec3 a_vCamPos = vec3(3.0f, 2.0f, 5.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f), const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false);
	virtual void Update(GLdouble a_dDeltaTime);
	virtual void CleanUpWin();
	virtual void Draw();
	virtual void MoveCamera(float a_fDeltaTime);
	virtual void Use();

	void CreateBuffer();
	void LoadShaders();
	void SetUniform();
};
#endif //!_CLOTHSIMULATION_H_