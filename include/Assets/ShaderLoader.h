#pragma once

#include <GL/glew.h>
#include <string>

class ShaderLoader
{
private:
    std::string readShader(const char* filename);
    GLuint createShader(GLenum shaderType, std::string source, const char* shaderName);
public:
    GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);

};