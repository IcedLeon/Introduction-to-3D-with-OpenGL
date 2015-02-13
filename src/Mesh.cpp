//#include "Mesh.h"
//
//Mesh::Mesh(vector<mVertex> vertices, vector<GLuint> indices, vector<Texture> textures)
//{
//	this->m_voVertices	= vertices;
//	this->m_voIndices	= indices;
//	this->m_voTextures	= textures;
//
//	this->setupMesh();
//}
//
//void Mesh::setupMesh()
//{
//	glGenVertexArrays(1, &this->VAO);
//	glGenBuffers(1, &this->VBO);
//	glGenBuffers(1, &this->IBO);
//
//	glBindVertexArray(this->VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//
//	glBufferData(GL_ARRAY_BUFFER, this->m_voVertices.size() * sizeof(mVertex),
//		&this->m_voVertices[0], GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_voIndices.size() * sizeof(GLuint),
//		&this->m_voIndices[0], GL_STATIC_DRAW);
//
//	// Vertex Positions
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mVertex),
//		(GLvoid*)0);
//	// Vertex Normals
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mVertex),
//		(GLvoid*)offsetof(mVertex, m_vNormal));
//	// Vertex Texture Coords
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mVertex),
//		(GLvoid*)offsetof(mVertex, m_vTextCoord));
//
//	glBindVertexArray(0);
//}
//
//void Mesh::Draw(ShaderCompiler m_glslShader)
//{
//	GLuint diffuseNr = 1;
//	GLuint specularNr = 1;
//	for (GLuint i = 0; i < this->m_voTextures.size(); i++)
//	{
//		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
//		// Retrieve texture number (the N in diffuse_textureN)
//		std::stringstream ss;
//		string number;
//		string name = this->m_voTextures[i].m_sType;
//		if (name == "texture_diffuse")
//			ss << diffuseNr++; // Transfer GLuint to stream
//		else if (name == "texture_specular")
//			ss << specularNr++; // Transfer GLuint to stream
//		number = ss.str();
//
//		glUniform1f(glGetUniformLocation(m_glslShader.GetShaderProgram(), ("material." + name + number).c_str()), (float)i);
//		glBindTexture(GL_TEXTURE_2D, this->m_voTextures[i].m_uiId);
//	}
//	glActiveTexture(GL_TEXTURE0);
//
//	// Draw mesh
//	glBindVertexArray(this->VAO);
//	glDrawElements(GL_TRIANGLES, this->m_voIndices.size(), GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//}