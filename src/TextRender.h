#ifndef _TEXTRENDER_H_
#define _TEXTRENDER_H_
//Libs
#include "BaseInclude.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "ShaderProgram.h"

using std::string;
using std::map;
using glm::ivec2;

struct Character
{
	GLuint	m_uiTextureID;   // ID handle of the glyph texture
	ivec2	m_vSize;    // Size of glyph
	ivec2	m_vBearing;  // Offset from baseline to left/top of glyph
	GLuint	m_uiAdvance;    // Horizontal offset to advance to next glyph
};

class Text
{
private:
	FT_Library				m_oFT;
	FT_Face					m_oFontFace;
	Character				m_oCharacter;
	map<GLchar, Character>	m_mCharacters;
	ShaderProgram			m_oTexProgram;
	GLuint					m_uiTVAO;
	GLuint					m_uiTVBO;

public:
	Text() {}
	~Text() {}

	void InitTextRendering(int a_iFontSize, const char* a_pccFontPath, vec2 a_vScreenDim);

	void RenderText(string a_sText,	
					vec2 a_vTextPos,
					GLfloat a_fScale,
					vec3 a_vColour);
};
#endif //!_TEXTRENDER_H_