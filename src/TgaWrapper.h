#ifndef _TGAWRAPPER_H_
#define _TGAWRAPPER_H_
//Lib
#include "gl_core_4_4.h"
#include <stdexcept>
#include <string>

#pragma warning(disable: 4290)

//Using macro
using std::string;
//Class forwarding
class ifstream;
class ofstream;

namespace TGAWRAP
{
	class IOExeption : public std::runtime_error
	{
	public:
		IOExeption(const string& a_sMSG) : std::runtime_error(a_sMSG) { }
	};
	namespace LITTLE_ENDIAN
	{
		//Read a little-endian short (2 bytes) from stream and return the value as an integer
		//Paramater:
		//<a_Stream>: the stream that you read from.
		int ReadShort(ifstream& a_Stream);
		//Write an integer as a little-endian short from to a stream
		//Paramater:
		//<a_Stream>: the stream to write to.
		//<a_iValue>: value to assign to.
		void WriteShort(ofstream& a_Stream, int a_iValue);
		//Read a little-endian integer (4 bytes) from stream and return the value.
		//Paramater:
		//<a_Stream>: the stream that you read from.
		int ReadInt(ifstream& a_Stream);
	}
	//Read from a .tga file
	//Paramater:
		//<a_pccFileName>: Path to .tga file.
		//<a_iWidth>: Reference (out), it'll set the integer to the native width of the file.
		//<a_iWidth>: Reference (out), it'll set the integer to the native height of the file.
	GLubyte* Read(const char* a_pccFileName, int& a_iWidth, int& a_iHeight) throw(IOExeption);
	//Write to a .tga file
	//Paramater:
		//<a_ubPixelData>: The data extracted from the file as a unsigned byte.
		//<a_iWidth>: Set the width of the texture.
		//<a_iWidth>: Set the height of the texture.
		//<a_pccFileName>: Path to .tga file.
	void Write(GLubyte* a_ubPixelData, int a_iWidth, int a_iHeight, const char* a_pccFileName) throw(IOExeption);
	//Loads a TGA file into an OpenGL texture.  This method only supports 24 or 32 bpp images. It yeld the texture ID.
	GLuint LoadTexture(const char* a_pccFileName, int& a_iWidth, int& a_iHeight);
	//Loads a TGA file into an OpenGL texture.  This method only supports 24 or 32 bpp images. It yeld the texture ID.
	//Paramater:
		//<a_pccFileName>: the file name of the TGA file.
	GLuint LoadTexture(const char* a_pccFileName);
}
#endif //!_TGAWRAPPER_H_