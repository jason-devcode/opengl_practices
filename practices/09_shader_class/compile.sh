#!/bin/env bash
g++ main.cpp ~/.gldev/glad/src/glad.c -o main -I ~/.gldev/glad/include -I ~/.gldev/glfw/include/ -L ~/.gldev/glfw/lib -lglfw3
