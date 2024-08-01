#include "Assets/ShaderLoader.h"

#include <iostream>
#include <fstream>
#include <vector>

std::string ShaderLoader::readShader(const char* filename)
{
    
    std::string shaderCode;
    std::ifstream file(filename, std::ios::in);
    
    if(!file.good())
    {
        std::cout << "Can't read file" << filename << std::endl;
        std::terminate();
    }

    // 这里整体的流程是，从输入流里面找到结束位置，然后从结束的位置开始计算出总的字符个数
    // 字符个数算出来了，把shaderCoderesize。
    // 然后返回到输入流的最开始位置
    // 把流的内容按照大小输入到shaderCode里面去

    // Sets the position of the next character to be extracted from the input stream.
    file.seekg(0, std::ios::end);

    //Returns the position of the current character in the input stream.
    shaderCode.resize((unsigned int)file.tellg());
    file.seekg(0, std::ios::beg);
    // 这种取string的第一位然后转化为地址的方式，就是变成c的char指针
    // Extracts n characters from the stream and stores them in the array pointed to by s.
    file.read(&shaderCode[0], shaderCode.size());
    file.close();
    return shaderCode;
}

GLuint ShaderLoader::createShader(GLenum shaderType, std::string source, const char* shaderName)
{
    int compileResult = 0;
    GLuint shader = glCreateShader(shaderType);
    const char* shaderCodePtr = source.c_str();

    const int shaderCodeSize = source.size();
    glShaderSource(shader, 1, &shaderCodePtr, &shaderCodeSize);
    glCompileShader(shader);
    // iv是什么
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

    if(compileResult == GL_FALSE)
    {
        int infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> shaderLog(infoLogLength);
        glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderLog[0]);
        std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shaderLog[0] << std::endl;
        return 0;
    }
    return shader;
}

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFileName, const char* fragmentShaderFilename)
{
    std::string vertexShaderCode = readShader(vertexShaderFileName);
    std::string fragmentShaderCode = readShader(fragmentShaderFilename);

    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

    int linkResult = 0;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    if(linkResult == GL_FALSE)
    {
        int infoLogLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> programLog(infoLogLength);
        glGetProgramInfoLog(program, infoLogLength, NULL, &programLog[0]);
        std::cout <<"Shader Loader: LINK ERROR" << std::endl << &programLog[0] << std::endl;
        return 0;
    }
    return program;
}