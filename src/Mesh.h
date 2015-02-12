//#ifndef _MESH_H_
//#define _MESH_H_
////Libs
////Window and GL
//#include "gl_core_4_4.h"
////Math
//#define GLM_SWIZZLE
//#include "glm/glm.hpp"
//#include "glm/ext.hpp"
////Other
//#include "ShaderCompiler.h"
//#include "Importer.hpp"
//#include "scene.h"
//#include "postprocess.h"
////std
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <vector>
//
//using glm::vec2;
//using glm::vec4;
//using std::string;
//using std::vector;
//
//struct mVertex
//{
//	vec4 m_vPos;
//	vec4 m_vNormal;
//	vec2 m_vTextCoord;
//};
//
//struct Texture {
//	GLuint m_uiId;
//	string m_sType; 
//	aiString path;
//};
//
//class Mesh
//{
//public:
//	/*  Mesh Data  */
//	vector<mVertex> m_voVertices;
//	vector<GLuint>	m_voIndices;
//	vector<Texture> m_voTextures;
//	/*  Functions  */
//	Mesh(vector<mVertex> vertices, vector<GLuint> indices, vector<Texture> textures);
//	void Draw(ShaderCompiler m_glslShader);
//private:
//	/*  Render data  */
//	GLuint VAO, VBO, IBO;
//	/*  Functions    */
//	void setupMesh();
//};
//#endif //!_MESH_H_