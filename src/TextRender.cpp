#include "TextRender.h"
#include "ShaderCompiler.h"

void Text::InitTextRendering(int a_iFontSize, const char* a_pccFontPath)
{
	//It return a different value from 0 if an error occur during initialization.
	if (FT_Init_FreeType(&m_oFT))
	{
		printf("ERROR: <FREETYPE LIBRARY COULD NOT BE INITIALIZED CORRECTLY. \n");
	}
	//Load the font as facen
	if (FT_New_Face(m_oFT, a_pccFontPath, NULL, &m_oFontFace))
	{
		printf("ERROR: <FREETYPE FAILED TO LOAD CURRENT FONT %s\n", a_pccFontPath);
	}
	//Set the size of the glyphs.
	FT_Set_Pixel_Sizes(m_oFontFace, NULL, a_iFontSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Load the first 128 characters of ASCII Set
	for (GLubyte chr = 0; chr < 128; ++chr)
	{
		//load char glyph
		if (FT_Load_Char(m_oFontFace, chr, FT_LOAD_RENDER))
		{
			printf("ERROR: <FREETYPE FAILED TO LOAD GLYPH. %c\n", chr);
			continue;
		}
		GLuint _texture;
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			NULL,
			GL_RED,
			m_oFontFace->glyph->bitmap.width,
			m_oFontFace->glyph->bitmap.rows,
			NULL,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_oFontFace->glyph->bitmap.buffer);
		//Texture parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
		m_oCharacter =
		{
			_texture,
			ivec2(m_oFontFace->glyph->bitmap.width, m_oFontFace->glyph->bitmap.rows),
			ivec2(m_oFontFace->glyph->bitmap_left, m_oFontFace->glyph->bitmap_top),
			m_oFontFace->glyph->advance.x
		};
		m_mCharacters.insert(std::pair<GLchar, Character>(chr, m_oCharacter));
	}
	glBindBuffer(GL_TEXTURE_2D, NULL);
	FT_Done_Face(m_oFontFace);
	FT_Done_FreeType(m_oFT);
	//Init gl buffer
	glGenVertexArrays(1, &m_uiTVAO);
	glGenBuffers(1, &m_uiTVBO);
	glBindVertexArray(m_uiTVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiTVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(sizeof(glm::vec4) * 2));
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);
}

void Text::RenderText(ShaderCompiler* a_iShaderProg, string a_sText, glm::vec2 a_vTextPos, GLfloat a_fScale, glm::vec3 a_VColour)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLuint _textColourUni = glGetUniformLocation(a_iShaderProg->GetShaderProgram(), "textColour");
	glUniform3f(_textColourUni, a_VColour.x, a_VColour.y, a_VColour.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_uiTVAO);
	string::const_iterator chr;

	for (chr = a_sText.begin(); chr != a_sText.end(); ++chr)
	{
		Character _currChar = m_mCharacters[*chr];

		GLfloat _posX = a_vTextPos.x + _currChar.m_vBearing.x * a_fScale;
		GLfloat _posY = a_vTextPos.y - (_currChar.m_vSize.y - _currChar.m_vBearing.y) * a_fScale;

		GLfloat _width	= _currChar.m_vSize.x * a_fScale;
		GLfloat _height = _currChar.m_vSize.y * a_fScale;

		GLfloat _verticies[6][4] = 
		{
			{ _posX,		  _posY + _height,	0.0f, 0.0f },
			{ _posX,		  _posY,			0.0f, 1.0f },
			{ _posX + _width, _posY + _height,	1.0f, 1.0f },

			{ _posX,		  _posY + _height,	0.0f, 0.0f },
			{ _posX + _width, _posY,			1.0f, 1.0f },
			{ _posX + _width, _posY + _height,  1.0f, 0.0f },
		};
		glBindTexture(GL_TEXTURE_2D, _currChar.m_uiTextureID);

		glBindBuffer(GL_ARRAY_BUFFER, m_uiTVBO);
		glBufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(_verticies), _verticies);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		a_vTextPos.x += (_currChar.m_uiAdvance >> 6) * a_fScale;
	}
	glBindVertexArray(NULL);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glDisable(GL_BLEND);
}