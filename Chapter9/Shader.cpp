#include "Shader.h"
#include "Math.h"
#include <sstream>
#include <fstream>
#include <SDL.h>

bool Shader::CompileShader(const std::string& file, GLenum shaderType, GLuint& outShader)
{
    std::ifstream ifs(file);
    
    if (!ifs.is_open())
    {
        SDL_Log("Shader file not found: %s", file.c_str());
        return false;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string contents = ss.str();
    const char* contentsChar = contents.c_str();

    outShader = glCreateShader(shaderType);
    glShaderSource(outShader, 1, &(contentsChar), nullptr);
    glCompileShader(outShader);

    if (!IsCompiled(outShader))
    {
        SDL_Log("Failed to compile shader %s", file.c_str());
        return false;
    }

    return true;
}

bool Shader::IsCompiled(GLuint shader)
{
    GLint status;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetShaderInfoLog(shader, 511, nullptr, buffer);
        SDL_Log("GLSL Compile Failed:\n%s", buffer);
        return false;
    }

    return true;
}

bool Shader::isValidProgram()
{
    GLint status;

    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
        SDL_Log("Shader Program Link Failed:\n%s", buffer);
        return false;
    }

    return true;
}

Shader::Shader()    :
    mVertexShader{ 0 },
    mFragShader{ 0 },
    mShaderProgram{ 0 }
{
}

Shader::~Shader()
{
}

bool Shader::Load(const std::string& vertFile, const std::string& fragFile)
{
    if (!CompileShader(vertFile, GL_VERTEX_SHADER, mVertexShader) ||
        !CompileShader(fragFile, GL_FRAGMENT_SHADER, mFragShader))
    {
        return false;
    }

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);

    if (!isValidProgram())
    {
        return false;
    }

    return true;
}

void Shader::Unload()
{
    glDeleteProgram(mShaderProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragShader);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);

    glUniformMatrix4fv(
        loc,
        1,
        GL_TRUE,
        matrix.GetAsFloatPtr()
    );
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);

    glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char* name, float value)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform1f(loc, value);
}

void Shader::SetActive()
{
    glUseProgram(mShaderProgram);
}
