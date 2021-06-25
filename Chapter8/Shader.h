#pragma once
#include <string>
#include <glew.h>

class Shader
{
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;

	bool CompileShader(const std::string& file, GLenum shaderType, GLuint& outShader);
	bool IsCompiled(GLuint shader);
	bool isValidProgram();

public:
	Shader();
	~Shader();

	bool Load(const std::string& vertFile, const std::string& fragFile);
	void Unload();
	void SetMatrixUniform(const char* name, const class Matrix4& matrix);
	void SetVectorUniform(const char* name, const class Vector3& vector);
	void SetFloatUniform(const char* name, float value);

	void SetActive();
};

