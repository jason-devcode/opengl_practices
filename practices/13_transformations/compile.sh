#!/bin/env bash
GL_LIBS="$HOME/.gldev"
GLM_INCLUDE="$GL_LIBS/glm/include"
GLM_LIB="$GL_LIBS/glm/lib"

GLAD_INCLUDE="$GL_LIBS/glad/include"
GLAD_SRC="$GL_LIBS/glad/src/glad.c"

GLFW_INCLUDE="$GL_LIBS/glfw/include"
GLFW_LIB="$GL_LIBS/glfw/lib"

g++ main.cpp "$GLAD_SRC" -o main \
    -I "$GLFW_INCLUDE" \
    -I "$GLAD_INCLUDE" \
    -I "$GLM_INCLUDE" \
    -L "$GLFW_LIB" \
    -L "$GLM_LIB" \
    -lglfw3 -lglm
