#include "TextureLoader.h"
#include <fstream>
#include <filesystem>

using std::ifstream;
using std::ofstream;

namespace TEXLOADER
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

	namespace TGA
	{
		GLubyte* ReadTGA(const char* a_pccFileName, int& a_iWidth, int& a_iHeight) throw(IOExeption)
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

					_idLenght = _inFile.get();
					_mapType = _inFile.get();
					_typeCode = _inFile.get();
					_inFile.ignore(5); //Ignore the color map info.
					_originX = LITTLE_ENDIAN::ReadShort(_inFile);
					_originY = LITTLE_ENDIAN::ReadShort(_inFile);
					a_iWidth = LITTLE_ENDIAN::ReadShort(_inFile); //Image width.
					a_iHeight = LITTLE_ENDIAN::ReadShort(_inFile); //Image height.
					_bpp = _inFile.get();
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
					printf("FileName: %s\n"
						"Width and Height: (%d x %d)\n"
						"BPP: %d\n "
						"OriginX and OriginY: (%d x %d)\n",
						a_pccFileName,
						a_iWidth, a_iHeight,
						_bpp,
						_originX, _originY);
					//Read the pixel data
					if (a_iWidth == NULL || a_iHeight == NULL)
						printf("<WARNING>: Width: %d or Height: %d of this .tga file is equal to zero. \n", a_iWidth, a_iHeight);
					GLubyte* _pixel = new GLubyte[a_iWidth * a_iHeight * 4];
					//24 bpp -- Blue, Green, Red
					//32 bpp -- Blue, Green, Red, Alpha
					//_pixel -- Stored as RGBA.
					unsigned int _imageSize = a_iWidth * a_iHeight;
					for (unsigned int i = 0; i < _imageSize; ++i)
					{
						const int _chl = 4; //number of channel
						_pixel[i * _chl + 2]	 = _inFile.get(); //Red
						_pixel[i * _chl + 1] = _inFile.get(); //Blue
						_pixel[i * _chl] = _inFile.get(); //Green
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

		float PerlinIntepolationVal(const float a_pfVal0, const float a_pfVal1, const float a_pfT)
		{
			float _fT;

			_fT = (1.0f - cosf(a_pfT * 3.14159265359f)) / 2.0f;

			return a_pfVal0 * (1.0f - _fT) + a_pfVal1 * _fT;
		}

		float PerlinNoiseValue2D(int a_iWidth, int a_iHeight, int a_iX, int a_iY, const int a_iAmplitude, const int* a_pciData2D)
		{
			if (!a_pciData2D)
				return 0.0f;

			a_iX = a_iX % a_iWidth;
			a_iY = a_iY % a_iHeight;

			if (a_iX < 0)
				a_iX = (int)a_iWidth + a_iX;
			if (a_iY < 0)
				a_iY = (int)a_iHeight + a_iY;

			return (float)(a_pciData2D[a_iX + a_iY * (int)a_iWidth] % a_iAmplitude);
		}

		GLubyte* ReadPerlinTGA(const char* a_pccFileName, int& a_iWidth, int& a_iHeight,
			const int a_iSeed, const float a_fFreq, const float a_fAmpli, const float a_fPers, const int a_iOct) throw(IOExeption)
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

					_idLenght = _inFile.get();
					_mapType = _inFile.get();
					_typeCode = _inFile.get();
					_inFile.ignore(5); //Ignore the color map info.
					_originX = LITTLE_ENDIAN::ReadShort(_inFile);
					_originY = LITTLE_ENDIAN::ReadShort(_inFile);
					a_iWidth = LITTLE_ENDIAN::ReadShort(_inFile); //Image width.
					a_iHeight = LITTLE_ENDIAN::ReadShort(_inFile); //Image height.
					_bpp = _inFile.get();
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
					printf("FileName: %s\n"
						"Width and Height: (%d x %d)\n"
						"BPP: %d\n "
						"OriginX and OriginY: (%d x %d)\n",
						a_pccFileName,
						a_iWidth, a_iHeight,
						_bpp,
						_originX, _originY);
					//Read the pixel data
					if (a_iWidth == NULL || a_iHeight == NULL)
						printf("<WARNING>: Width: %d or Height: %d of this .tga file is equal to zero. \n", a_iWidth, a_iHeight);

					GLubyte* _pixel = new GLubyte[a_iWidth * a_iHeight * 4];
					//24 bpp -- Blue, Green, Red
					//32 bpp -- Blue, Green, Red, Alpha
					//_pixel -- Stored as RGBA.
					unsigned int _imageSize = a_iWidth * a_iHeight;

					float* _data;
					int* _data2D;

					float _frqFact;
					float _ampFact;

					if (a_fPers <= 0.0f || a_iWidth < 1 || a_iHeight < 1)
						return NULL;

					_data = new float[a_iWidth * a_iHeight * sizeof(float)];

					for (unsigned int i = 0; i < _imageSize; ++i)
					{
						_data[i] = 0.0f;
						const int _chl = 4; //number of channel
						_pixel[i * _chl + 2] = _inFile.get(); //Red
						_pixel[i * _chl + 1] = _inFile.get(); //Blue
						_pixel[i * _chl] = _inFile.get(); //Green
						if (_bpp == 32)
							_pixel[i * _chl + 3] = _inFile.get();
						else
							_pixel[i * _chl + 3] = 0xFF;
					}
					_data2D = new int[a_iWidth * a_iHeight * sizeof(int)];

					srand(a_iSeed);

					for (unsigned int i = 0; i < _imageSize; ++i)
						_data2D[i] = rand();

					_frqFact = 1.0f;
					_ampFact = 1.0f / a_fPers;

					for (int i = 0; i < a_iOct; ++i)
					{
						int _x, _y;

						int _waveLenghtX,
							_waveLenghtY;

						float _currFrq = a_fFreq * _frqFact;
						float _curAmp = a_fAmpli / (1.0f * _ampFact);

						if (_currFrq <= 1.0f)
							_currFrq = 1.0f;

						_waveLenghtX = a_iWidth / (int)_currFrq;
						_waveLenghtY = a_iHeight / (int)_currFrq;

						if (_waveLenghtX < 1)
							_waveLenghtX = 1;
						if (_waveLenghtY < 1)
							_waveLenghtY = 1;

						for (_y = 0; _y < a_iHeight; _y += _waveLenghtY)
						{
							for (_x = 0; _x < a_iWidth; _x += _waveLenghtX)
							{
								int _xInner, _yInner;

								float _p[4];

								int _xRand = _x / _waveLenghtX;
								int _yRand = _y / _waveLenghtY;

								_p[0] = PerlinNoiseValue2D(a_iWidth, a_iHeight, _xRand, _yRand, (int)_curAmp, _data2D);
								_p[1] = PerlinNoiseValue2D(a_iWidth, a_iHeight, _xRand + 1, _yRand, (int)_curAmp, _data2D);
								_p[2] = PerlinNoiseValue2D(a_iWidth, a_iHeight, _xRand, _yRand + 1, (int)_curAmp, _data2D);
								_p[3] = PerlinNoiseValue2D(a_iWidth, a_iHeight, _xRand + 1, _yRand + 1, (int)_curAmp, _data2D);

								for (_yInner = 0; _yInner < _waveLenghtY && _y + _yInner < a_iHeight; ++_yInner)
								{
									for (_xInner = 0; _xInner < _waveLenghtX && _x + _xInner < a_iWidth; ++_xInner)
									{
										int _index = (_y + _yInner) * a_iWidth + _x + _xInner;

										float _x0 = PerlinIntepolationVal(_p[0], _p[1], (float)_xInner / (float)_waveLenghtX);
										float _x1 = PerlinIntepolationVal(_p[2], _p[3], (float)_xInner / (float)_waveLenghtX);

										_data[_index] += PerlinIntepolationVal(_x0, _x1, (float)_yInner / (float)_waveLenghtY);
									}
								}
							}
						}

						_frqFact *= 2.0f;
						_ampFact *= 1.0f / a_fPers;
					}
					delete[] _data2D;

					for (int i = 0; i = _imageSize; ++i)
						_pixel[i] = (GLubyte)_data[i];
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

		void WriteTGA(GLubyte* a_ubPixelData, int a_iWidth, int a_iHeight, const char* a_pccFileName) throw(IOExeption)
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

					if (a_iWidth == NULL || a_iHeight == NULL)
						printf("<WARNING>: Width: %d or Height: %d of this .tga file is equal to zero. \n", a_iWidth, a_iHeight);
					int _imageSize = a_iWidth * a_iHeight;
					for (int i = 0; i < _imageSize; ++i)
					{
						const int _chl = 4;
						_ofFile.put(a_ubPixelData[i * _chl + 2]); //Blue
						_ofFile.put(a_ubPixelData[i * _chl + 1]); //Green
						_ofFile.put(a_ubPixelData[i * _chl]); //Red
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
	}

	namespace BMP
	{
		GLubyte* ReadBMP(const char* a_pccFileName, int& a_iWidth, int& a_iHeight) throw(IOExeption)
		{
			char _header[3];
			int _fileSize, 
				_offset, 
				_numCols, 
				_numRows;
			int bpp;

			std::ifstream inFile(a_pccFileName, std::ios::binary);
			if (!inFile) {
				printf("<ERROR>: Can't open file: %s\n", a_pccFileName);
				return NULL;
			}

			inFile.get(_header, 3);
			string magic(_header);
			if (magic != "BM") {
				printf("Unrecognized file format: %s\n", a_pccFileName);
				inFile.close();
				return NULL;
			}

			_fileSize	= TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);   // Total file size
			TEXLOADER::LITTLE_ENDIAN::ReadShort(inFile);            // ignore, reserved
			TEXLOADER::LITTLE_ENDIAN::ReadShort(inFile);            // ignore, reserved
			_offset		= TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);     // offset to image
			TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);              // Header size
			_numCols	= TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);    // Number of columns in image (width)
			_numRows	= TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);    // Number of rows in image (height)

			TEXLOADER::LITTLE_ENDIAN::ReadShort(inFile);            // Should always be 1
			bpp = TEXLOADER::LITTLE_ENDIAN::ReadShort(inFile);      // Bits per pixel, we support 24 or 32

			TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);              // Compression method
			TEXLOADER::LITTLE_ENDIAN::ReadInt(inFile);              // total size of bitmap data

			if (bpp != 24 && bpp != 32) {
				printf("Error: %s is not a 32 or 24 bits per pixel image.\n", a_pccFileName);
				inFile.close();
				return NULL;
			}

			// If the image height (numRows) is negative, rows are stored top to
			// bottom, so we need to flip the image vertically.
			bool _flipImage = false;
			if (_numRows < 0) 
			{
				_flipImage	= true;
				_numRows	= -_numRows;
			}

			printf("Loading:\n"
				   "File: %s\n"
				   "BPP: %d\n"
				   "Dimension: %d x %d\n"
				   "FileSize: %d bytes\n"
				   "Order: is %s\n", 
				   a_pccFileName, 
				   bpp, 
				   _numCols, _numRows, 
				   _fileSize,
				   _flipImage ? "top to bottom" : "bottom to top");

			// add bytes at end of each row so total # is a multiple of 4
			// round up 3 * numCols to next mult. of 4
			int _numPadBytes	= 0, nBytesInRow;
			nBytesInRow			= (bpp * _numCols) / 8;
			if (nBytesInRow % 4 != 0) _numPadBytes = 4 - nBytesInRow % 4;

			a_iHeight	= _numRows;
			a_iWidth	= _numCols;
			GLubyte * pixelData = new GLubyte[a_iWidth*a_iHeight * 4];
			if (!pixelData) {
				printf("Error: out of memory!\n");
				inFile.close();
				return NULL;
			}

			// Skip to the beginning of the pixel data
			inFile.seekg(_offset);

			// Load pixels
			char pixBytes[4];
			for (int row = 0; row < _numRows; row++) {
				for (int col = 0; col < _numCols; col++) {
					if (bpp == 32)
						inFile.read(pixBytes, 4);
					else {
						inFile.read(pixBytes, 3);
						pixBytes[3] = { (const char)255 };    // set alpha to 255
					}

					int rowOut = row;
					if (_flipImage) rowOut = _numRows - 1 - row;

					int idx = ((rowOut * _numCols) + col) * 4;
					pixelData[idx] = pixBytes[2]; // Red
					pixelData[idx + 1] = pixBytes[1]; // Green
					pixelData[idx + 2] = pixBytes[0]; // Blue
					pixelData[idx + 3] = pixBytes[3]; // Alpha
				}
				// skip pad bytes at row end
				inFile.read(pixBytes, _numPadBytes);
			}

			inFile.close();
			return pixelData;
		}
	}

	GLuint LoadTexture(const char* a_pccFileName, int& a_iWidth, int& a_iHeight, bool a_bInitTex)
	{
		char _sep = '/';
//#ifdef _WIN32
//		_sep = '\\';
//#endif
		string _finalExt;
		string _ext(a_pccFileName);
		size_t _id = _ext.rfind(_sep, _ext.length());
		if (_id != string::npos)
		{
			_finalExt = _ext.substr(_id + 1, _ext.length() - _id);

			_sep = '.';
			_ext = _finalExt;
			_id = _ext.rfind(_sep, _ext.length());
			
			if (_id != string::npos)
			{
				_finalExt = _ext.substr(_id + 1, _ext.length() - _id);
			}
		}
		//GL texture handle. (This will be returned as the final value.
		GLuint	_textureID;
		//Image data
		GLubyte* _data = new GLubyte;
		if (_finalExt == "tga") 
			_data = TEXLOADER::TGA::ReadTGA(a_pccFileName, a_iWidth, a_iHeight); 
		if (_finalExt == "bmp")
			_data = TEXLOADER::BMP::ReadBMP(a_pccFileName, a_iWidth, a_iHeight);
		if (a_bInitTex)
		{
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
		}
		//Delete the texture data, since it's already been created and copy over.
		delete[] _data;
		return _textureID;
	}
	
	GLuint LoadTextureMipMap(const char* a_pccFileName, int& a_iWidth, int& a_iHeight)
	{
		char _sep = '/';
		//#ifdef _WIN32
		//		_sep = '\\';
		//#endif
		string _finalExt;
		string _ext(a_pccFileName);
		size_t _id = _ext.rfind(_sep, _ext.length());
		if (_id != string::npos)
		{
			_finalExt = _ext.substr(_id + 1, _ext.length() - _id);

			_sep = '.';
			_ext = _finalExt;
			_id = _ext.rfind(_sep, _ext.length());

			if (_id != string::npos)
			{
				_finalExt = _ext.substr(_id + 1, _ext.length() - _id);
			}
		}
		//GL texture handle. (This will be returned as the final value.
		GLuint	_textureID;
		//Image data
		GLubyte* _data = new GLubyte;
		if (_finalExt == "tga")
			_data = TEXLOADER::TGA::ReadTGA(a_pccFileName, a_iWidth, a_iHeight);
		if (_finalExt == "bmp")
			_data = TEXLOADER::BMP::ReadBMP(a_pccFileName, a_iWidth, a_iHeight);

		//Generate and Bind the texture handle.
		glGenTextures(1, &_textureID);
		glBindTexture(GL_TEXTURE_2D, _textureID);
		//Allocate the storage
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, _data);
		//Mipmap generation
		glGenerateMipmap(GL_TEXTURE_2D);
		//Set texture parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//Unbind
		glBindTexture(GL_TEXTURE_2D, NULL);
		//Delete the texture data, since it's already been created and copy over.
		delete[] _data;
		return _textureID;
	}

	GLuint LoadTexture(const char* a_pccFileName, bool a_bInitTex, bool a_bMipMap)
	{
		GLint _w, _h;
		if (!a_bMipMap)
			return LoadTexture(a_pccFileName, _w, _h, a_bInitTex);
		else
			return LoadTextureMipMap(a_pccFileName, _w, _h);
	}
}