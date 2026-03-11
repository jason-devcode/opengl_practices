#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logger.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback( GLFWwindow*, int width, int height ) {
    glViewport( 0, 0, width, height );
}

void process_inputs( GLFWwindow* window ) {

    // If key escape was pressed then window should close
    if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
        glfwSetWindowShouldClose( window, true );

    // Toggle wireframe mode
    if( glfwGetKey( window, GLFW_KEY_1 ) == GLFW_PRESS )
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if( glfwGetKey( window, GLFW_KEY_2 ) == GLFW_PRESS )
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

GLFWwindow* init_engine( int width, int height, const char* title) {
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow( width, height, title, NULL, NULL);

    if( !window ) {
        LOG_ERROR("Failed to create window!");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent( window );
    glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );

    if( not gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress ) ) {
        LOG_ERROR("Failed to load GL Loader!");
        glfwDestroyWindow( window );
        glfwTerminate();
        return NULL;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return window;
}

void close_engine( GLFWwindow* window ) {
    if( window ) glfwDestroyWindow( window );
    glfwTerminate();
}

#endif