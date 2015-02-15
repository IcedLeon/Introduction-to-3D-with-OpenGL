#ifndef _CONSTANT_H_
#define _CONSTANT_H_
//Lib
//Window and GL
#include "gl_core_4_4.h"
//Math
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::lookAt;
using glm::radians;
using glm::rotation;
using glm::transpose;
using glm::perspective;
//Others
#include <thread>
//Windows name
#define WIN_NAME_PREFIX "OPENGL TEST"
//Shader info lenght
#define SHD_ERR_LGH 512
//////////////////////
// Shader list path //
//////////////////////
//Geometry
#define GEOMETRY_VERTEX_GLSL	"./shaders/VertexShader.glsl"
#define GEOMETRY_FRAGMENT_GLSL	"./shaders/FragmentShader.glsl"
//Light shaders
#define LIGHTING_VERTEX_GLSL	"./shaders/LightingVertex.glsl"
#define LIGHTING_FRAGMENT_GLSL	"./shaders/LightingFragment.glsl"
//Advance texture shaders
#define TEXTURE_VERTEX_GLSL		"./shaders/TextureVertex.glsl"
#define TEXTURE_FRAGMENT_GLSL	"./shaders/TextureFragment.glsl"
//Basic colour output.
//static vec4 m_vWhite	= vec4(1.0f);
//static vec4 m_vBlack	= vec4(0.0f, 0.0f, 0.0f, 1.0f);
//static vec4 m_vRed		= vec4(1.0f, 0.0f, 0.0f, 1.0f);
//static vec4 m_vGreen	= vec4(0.0f, 1.0f, 0.0f, 1.0f);
//static vec4 m_vBlue		= vec4(0.0f, 0.0f, 1.0f, 1.0f);

#endif //!_CONSTANT_H_