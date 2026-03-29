#ifndef SHADER_H
#define SHADER_H

#define LOG_USE_COLOR

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "logger.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

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

class Shader
{
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
    ~Shader();

    void use();

    GLuint getUniformLocation( const char* uniform_name );

    void setVec3( const char* uniform_name, vec3& input ) {
        GLuint u_loc = glGetUniformLocation( program, uniform_name );
        glUniform3fv( u_loc, 1, glm::value_ptr(input));
    }

    void setFloat( const char* uniform_name, float input ) {
        GLuint u_loc = glGetUniformLocation( program, uniform_name );
        glUniform1f( u_loc, input );
    }

    void setMat4( const char* uniform_name, mat4& input ) {
        GLuint u_loc = glGetUniformLocation( program, uniform_name );
        glUniformMatrix4fv( u_loc, 1, GL_FALSE, glm::value_ptr( input ) );
    }

    void setTexUnit( const char* tex_uniform_name, int tex_unit ) {
        GLuint u_loc = glGetUniformLocation( program, tex_uniform_name );
        glUniform1i(u_loc, tex_unit);
    }

    GLboolean success = GL_FALSE;
    GLuint program = 0;
private:
    std::string read_shader_file(const char *filepath);
    CompileResult compile_shader(const char *sourceCode, GLenum shader_type);
    LinkResult link_shaders( GLuint vertexShader, GLuint fragmentShader );
};

GLuint Shader::getUniformLocation( const char* uniform_name ) {
    return glGetUniformLocation( program, uniform_name );
}

void Shader::use() {
    if( program != -1 )
        glUseProgram( program );
}

Shader::~Shader() {
    glDeleteProgram( program );
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
    program = programResult.program;
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

CompileResult Shader::compile_shader(const char *sourceCode, GLenum shader_type)
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
        return (CompileResult){ 0, GL_FALSE};
    }
    LOG_SUCCESS( "%s shader compiled successfully!", shaderLogType );

    return (CompileResult){ shader, GL_TRUE};
}

LinkResult Shader::link_shaders( GLuint vertexShader, GLuint fragmentShader ) {
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
        
        return (LinkResult){ 0, GL_FALSE };
    }
    LOG_SUCCESS( "Shaders linked successfully!" );
    return (LinkResult){ program, GL_TRUE };
}


#endif