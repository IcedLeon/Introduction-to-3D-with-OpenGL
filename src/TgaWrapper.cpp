#include "TgaWrapper.h"
#include <fstream>
#include <assert.h>

using std::ifstream;
using std::ofstream;

namespace TGAWRAP
{
	namespace LITTLE_ENDIAN
	{
		int ReadShort(ifstream& a_Stream)
		{
			unsigned char	_inChar; //input char
			int				_result;

			_inChar  = a_Stream.get();
			_result  = _inChar;
			_inChar  = a_Stream.get();
			_result |= ((unsigned int)_inChar << 8);
			return _result;
		}
		void WriteShort(ofstream& a_Stream, int a_iValue)
		{
			unsigned char _lowOrder		= (unsigned char)a_iValue;
			unsigned char _hightOrder	= (unsigned char)(a_iValue >> 8);

			a_Stream.put(_lowOrder);
			a_Stream.put(_hightOrder);
		}

		int ReadInt(ifstream& a_Stream)
		{
			int _result				= NULL;
			unsigned char _inChar	= NULL;

			_inChar  = a_Stream.get();
			_result  = _inChar;
			_inChar  = a_Stream.get();
			_result |= ((unsigned int)_inChar << 8);
			_inChar  = a_Stream.get();
			_result |= ((unsigned int)_inChar << 16);
			_inChar  = a_Stream.get();
			_result |= ((unsigned int)_inChar << 24);
			return _result;
		}
	} //End of LITTLE_ENDIAN

	GLubyte* Read(const char* a_pccFileName, int& a_iWidth, int& a_iHeight) throw(IOExeption)
	{
		//Open the file for reading data
		ifstream _inFile(a_pccFileName, std::ios::binary);

		try
		{
			if (!_inFile)
			{
				string _msg = string("<ERROR>: Could not open the file located at %s. Check again the inserted path \n", a_pccFileName);
				throw IOExeption(_msg);
			}
			else
			{
				int _idLenght, //Lenght of the image ID field.
					_mapType,  //Color map type (expect 0 == no color map).
					_typeCode, //Image type code (expect 2 == uncompressed).
					_originX,  //UV coordinate of the origin for the X.
					_originY,  //UV coordinate of the origin for the Y.
					_bpp	   //Bits per pixel (expect 24 or 32).
					= NULL;

				_idLenght	= _inFile.get();
				_mapType	= _inFile.get();
				_typeCode	= _inFile.get();
				_inFile.ignore(5); //Ignore the color map info.
				_originX	= LITTLE_ENDIAN::ReadShort(_inFile);
				_originY	= LITTLE_ENDIAN::ReadShort(_inFile);
				a_iWidth	= LITTLE_ENDIAN::ReadShort(_inFile); //Image width.
				a_iHeight	= LITTLE_ENDIAN::ReadShort(_inFile); //Image height.
				_bpp		= _inFile.get();
				_inFile.ignore(); //Image descriptor (expect 0 for 24bpp and 8 for 32bpp).

#define UNCOMPRESSED_ERR "<ERROR>: File does not appear to be a non-color-mapped, uncompressed TGA image \n"
#define FILE_SIZE_ERR	 "<ERROR>: File must be 24 or 32 bits per pixel \n"
				//Check for uncompressed tga image file.
				if (_typeCode != 2 || _mapType != 0)
				{
					throw IOExeption(UNCOMPRESSED_ERR);
				}
				//Check for bite per pixel size exessing
				if (_bpp != 24 && _bpp != 32)
				{
					throw IOExeption(FILE_SIZE_ERR);
				}
				//Skip if the image data ID
				if (_idLenght > NULL)
					_inFile.ignore(_idLenght);
				//At this point the color map would be here, but we assume no color map (check answer again, keep fogetting why I should assume that...)
				printf("FileName: %s\n," 
					   "Width and Height: (%d x %d)\n," 
					   "BPP: %d\n, "
					   "OriginX and OriginY: (%d x %d)\n",
					   a_pccFileName,
					   a_iWidth, a_iHeight,
					   _bpp,
					   _originX, _originY);
				//Read the pixel data
				assert(a_iWidth != NULL || a_iHeight != NULL);
					printf("Width or Height of this .tga file is equal to zero. \n");
				GLubyte* _pixel = new GLubyte[a_iWidth * a_iHeight * 4];
				//24 bpp -- Blue, Green, Red
				//32 bpp -- Blue, Green, Red, Alpha
				//_pixel -- Stored as RGBA.
				unsigned int _imageSize = a_iWidth * a_iHeight;
				for (unsigned int i = 0; i < _imageSize; ++i)
				{
					const int _chl = 4; //number of channel
					_pixel[i * _chl + 2] = _inFile.get(); //Blue
					_pixel[i * _chl + 1] = _inFile.get(); //Green
					_pixel[i * _chl]	 = _inFile.get(); //Red
					if (_bpp == 32)
						_pixel[i * _chl + 3] = _inFile.get();
					else
						_pixel[i * _chl + 3] = 0xFF;

				}
				//Close down the file.
				_inFile.close();
				return _pixel;
			}
		}
		catch (IOExeption& a_Exeption)
		{
			_inFile.close();
			throw a_Exeption;
		}
	}

	void Write(GLubyte* a_ubPixelData, int a_iWidth, int a_iHeight, const char* a_pccFileName) throw(IOExeption)
	{
		ofstream _ofFile(a_pccFileName, std::ios::binary);

		try
		{
			if (!_ofFile)
			{
				string _msg = string("<ERROR>: Unable to open file at path: ") + 
							  a_pccFileName +
							  string("for writing. \n");
				throw IOExeption(_msg);
			}
			else
			{
				const char _zero[] = { 0, 0, 0, 0, 0 };

				_ofFile.put(0);									//Lenght of the image ID field.
				_ofFile.put(0);									//Color map type (expect 0 == no color map).
				_ofFile.put(2);									//Image type code (expect 2 == uncompressed).
				_ofFile.write(_zero, 5);						//Colour map info (ignored).
				_ofFile.write(_zero, 2);						//UV coordinate of the origin for the X.
				_ofFile.write(_zero, 2);						//UV coordinate of the origin for the Y.
				LITTLE_ENDIAN::WriteShort(_ofFile, a_iWidth);	//Image width.
				LITTLE_ENDIAN::WriteShort(_ofFile, a_iHeight);	//Image height.
				_ofFile.put(32);								//Bits per pixel (32).
				_ofFile.put(8);									//Image descriptor (8 => 32bpp).
				assert(a_iWidth != NULL || a_iHeight != NULL);
					printf("Width or Height of this .tga file is equal to zero. \n");
				int _imageSize = a_iWidth * a_iHeight;
				for (int i = 0; i < _imageSize; ++i)
				{
					const int _chl = 4;
					_ofFile.put(a_ubPixelData[i * _chl + 2]); //Blue
					_ofFile.put(a_ubPixelData[i * _chl + 1]); //Green
					_ofFile.put(a_ubPixelData[i * _chl	  ]); //Red
					_ofFile.put(a_ubPixelData[i * _chl + 3]); //Alpha
				}
				_ofFile.close();
			}
		}
		catch (IOExeption& a_Exeption)
		{
			_ofFile.close();
			throw a_Exeption;
		}
	}

	GLuint LoadTexture(const char* a_pccFileName, int& a_iWidth, int& a_iHeight)
	{
		//Image data
		GLubyte* _data = TGAWRAP::Read(a_pccFileName, a_iWidth, a_iHeight);
		//GL texture handle. (This will be returned as the final value.
		GLuint	_textureID;
		//Generate and Bind the texture handle.
		glGenTextures(1, &_textureID);
		glBindTexture(GL_TEXTURE_2D, _textureID);
		//Allocatehe the storage
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, a_iWidth, a_iHeight);
		//Copy the data into the storage.
		glTexSubImage2D(GL_TEXTURE_2D, NULL, NULL, NULL, a_iWidth, a_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, _data);
		//Set texture parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Delete the texture data, since it's already been created and copy over.
		delete[] _data;
		return _textureID;
	}
	
	GLuint LoadTexture(const char* a_pccFileName)
	{
		GLint _w, _h;
		return LoadTexture(a_pccFileName, _w, _h);
	}
}