#ifndef SHADER_H
#define SHADER_H

#define LOG_USE_COLOR

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>

#include "logger.h"

class Shader
{
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
    ~Shader();

    void use();

    GLuint getUniformLocation( const char* uniform_name );

    typedef struct
    {
        GLuint shader;
        GLint success;
    } CompileResult;
    
    typedef struct
    {
        GLuint program;
        GLint success;
    } LinkResult;

    GLboolean success = GL_FALSE;
    GLuint m_shader_program = 0;
private:
    std::string read_shader_file(const char *filepath);
    CompileResult compile_shader(const char *sourceCode, GLenum shader_type);
    LinkResult link_shaders( GLuint vertexShader, GLuint fragmentShader );
};

GLuint Shader::getUniformLocation( const char* uniform_name ) {
    return glGetUniformLocation( m_shader_program, uniform_name );
}

void Shader::use() {
    // if( m_shader_program != -1 )
        glUseProgram( m_shader_program );
}

Shader::~Shader() {
    glDeleteProgram( m_shader_program );
}

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // process vertex shader
    std::string vs_src = read_shader_file( vertexShaderPath );
    CompileResult vs_result = compile_shader( vs_src.c_str(), GL_VERTEX_SHADER );

    if( !vs_result.success ) {
        LOG_ERROR("Failed to process vertex shader");
        return;
    }

    // process fragment shader
    std::string fs_src = read_shader_file( fragmentShaderPath );
    CompileResult fs_result = compile_shader( fs_src.c_str(), GL_FRAGMENT_SHADER );

    if( !vs_result.success ) {
        LOG_ERROR("Failed to process fragment shader");
        return;
    }
    
    // Link shader program
    LinkResult programResult = link_shaders( vs_result.shader, fs_result.shader );

    if( !programResult.success ) {
        LOG_ERROR("Shader::Shader: Failed to link shader program");
        return;
    }

    // Set shader program member
    m_shader_program = programResult.program;
    success = GL_TRUE;
}

std::string Shader::read_shader_file(const char *filepath)
{
    std::string shaderContent;
    std::ifstream shaderFile;

    LOG_INFO( "Reading shader file %s...", filepath );

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
        LOG_ERROR( "%s", e.what() );
    }
    return NULL;
}

Shader::CompileResult Shader::compile_shader(const char *sourceCode, GLenum shader_type)
{
    const char* shaderLogType = (shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    LOG_INFO( "Compiling %s shader...", shaderLogType );

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    // check shader compilation status
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log_info[1024];
        glGetShaderInfoLog(shader, 1024, NULL, log_info);
        LOG_ERROR( "Failed to compile %s shader: %s", shaderLogType, log_info );

        glDeleteShader(shader);
        return (Shader::CompileResult){ 0, GL_FALSE};
    }
    LOG_SUCCESS( "%s shader compiled successfully!", shaderLogType );

    return (Shader::CompileResult){ shader, GL_TRUE};
}

Shader::LinkResult Shader::link_shaders( GLuint vertexShader, GLuint fragmentShader ) {
    LOG_INFO("Linking shaders...");

    GLuint program = glCreateProgram();
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );

    // Check link status
    GLint success = GL_FALSE;
    glGetProgramiv( program, GL_LINK_STATUS, &success );

    if( !success ) {
        char info_log[ 1024 ];
        glGetProgramInfoLog( program, 1024, NULL, info_log );

        LOG_ERROR( "Failed to link shader program: %s", info_log );
        
        return (Shader::LinkResult){ 0, GL_FALSE };
    }
    LOG_SUCCESS( "Shaders linked successfully!" );
    return (Shader::LinkResult){ program, GL_TRUE };
}


#endif