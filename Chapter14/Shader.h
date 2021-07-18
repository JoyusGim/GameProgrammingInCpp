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
	void SetMatrixUniforms(const char* name, class Matrix4* matrices, unsigned count);
	void SetVectorUniform(const char* name, const class Vector3& vector);
	void SetVector2Uniform(const char* name, const class Vector2& vector);
	void SetFloatUniform(const char* name, float value);
	void SetIntUniform(const char* name, int value);

	void SetActive();
};

