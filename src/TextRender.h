#ifndef _TEXTRENDER_H_
#define _TEXTRENDER_H_
//Libs
#include "gl_core_4_4.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <map>

using std::string;
using std::map;
using glm::ivec2;

class ShaderCompiler;

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
	GLuint					m_uiTVAO;
	GLuint					m_uiTVBO;
public:
	Text() {}
	~Text() {}

	void InitTextRendering(int a_iFontSize, const char* a_pccFontPath);

	void RenderText(ShaderCompiler* a_iShaderProg,
		string a_sText,
		glm::vec2 a_vTextPos,
		GLfloat a_fScale,
		glm::vec3 a_VColour);
};
#endif //!_TEXTRENDER_H_