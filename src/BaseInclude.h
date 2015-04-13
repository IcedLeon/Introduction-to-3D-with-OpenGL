#ifndef _BASEINCLUDE_H_
#define _BASEINCLUDE_H_
//Lib
//Window and GL
#include "gl_core_4_4.h"
//Math
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
//Others
#include <thread>
#include <map>
#include <string>
#include <memory>
//Using shortcut.
using std::map;
using std::string;
using std::shared_ptr;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::ivec2;
using glm::lookAt;
using glm::radians;
using glm::rotation;
using glm::transpose;
using glm::perspective;
//Macro
#define V_2_ZERO	vec2(0.0f, 0.0f);
#define V_3_ZERO	vec3(0.0f, 0.0f, 0.0f)
#define V_4_ZERO	vec4(0.0f, 0.0f, 0.0f, 0.0f)
#define M_IDENTITY	mat4(1.0f)
#define I_V_2_ZERO	ivec2(0, 0)
#define MAX_KEYS	1024

#endif //!_BASEINCLUDE_H_