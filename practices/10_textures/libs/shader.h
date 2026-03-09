#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>

class Shader
{
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);

    void use();

    GLuint getUniformLocation( const char* uniform_name );

    typedef struct
    {
        GLint shader;
        GLint success;
    } CompileResult;
    
    typedef struct
    {
        GLint program;
        GLint success;
    } LinkResult;

    GLint m_shader_program = -1;
private:
    std::string read_shader_file(const char *filepath);
    CompileResult compile_shader(const char *sourceCode, GLenum shader_type);
    LinkResult link_shaders( GLuint vertexShader, GLuint fragmentShader );
};

GLuint Shader::getUniformLocation( const char* uniform_name ) {
    return glGetUniformLocation( m_shader_program, uniform_name );
}

void Shader::use() {
    if( m_shader_program != -1 )
        glUseProgram( m_shader_program );
}

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // process vertex shader
    std::string vs_src = read_shader_file( vertexShaderPath );
    CompileResult vs_result = compile_shader( vs_src.c_str(), GL_VERTEX_SHADER );

    if( not vs_result.success ) {
        std::cerr << "Shader::Shader: Failed to process vertex shader\n";
        return;
    }

    // process fragment shader
    std::string fs_src = read_shader_file( fragmentShaderPath );
    CompileResult fs_result = compile_shader( fs_src.c_str(), GL_FRAGMENT_SHADER );

    if( not vs_result.success ) {
        std::cerr << "Shader::Shader: Failed to process fragment shader\n";
        return;
    }
    
    // Link shader program
    LinkResult programResult = link_shaders( vs_result.shader, fs_result.shader );

    if( not programResult.success ) {
        std::cerr << "Shader::Shader: Failed to link shader program\n";
        return;
    }

    // Set shader program member
    m_shader_program = programResult.program;
}

std::string Shader::read_shader_file(const char *filepath)
{
    std::string shaderContent;
    std::ifstream shaderFile;

    std::cout << "Shader::read_shader_file: Reading shader file " << filepath << "...\n";

    try
    {
        shaderFile.open(filepath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderContent = shaderStream.str();
        return shaderContent;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Shader::read_shader_file: " << e.what() << '\n';
    }
    return NULL;
}

Shader::CompileResult Shader::compile_shader(const char *sourceCode, GLenum shader_type)
{
    const char* shaderLogType = (shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    std::cout << "Shader::compile_shader: Compiling " << shaderLogType << " shader...\n";

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    // check shader compilation status
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (not success)
    {
        char log_info[1024];
        glGetShaderInfoLog(shader, 1024, NULL, log_info);

        std::cerr << "Shader::compile_shader: Failed to compile " << shaderLogType << " shader: \n";
        std::cerr << log_info << "\n";
        glDeleteShader(shader);
        return (Shader::CompileResult){-1, GL_FALSE};
    }
    std::cout << "Shader::compile_shader: " << shaderLogType << " shader compiled successfully\n";

    return (Shader::CompileResult){ shader, GL_TRUE};
}

Shader::LinkResult Shader::link_shaders( GLuint vertexShader, GLuint fragmentShader ) {
    std::cout << "Shader::link_shaders: Linking shaders...";

    GLuint program = glCreateProgram();
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );

    // Check link status
    GLint success = GL_FALSE;
    glGetProgramiv( program, GL_LINK_STATUS, &success );

    if( not success ) {
        char info_log[ 1024 ];
        glGetProgramInfoLog( program, 1024, NULL, info_log );
        std::cerr << "Shader::link_shaders: Failed to link shader program:\n" << info_log << "\n";
        return (Shader::LinkResult){ -1, GL_FALSE };
    }
    std::cout << "Shader::link_shaders: Shaders linked successfully\n";
    return (Shader::LinkResult){ program, GL_TRUE };
}


#endif