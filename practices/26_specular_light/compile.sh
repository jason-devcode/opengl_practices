#!/bin/bash

# ==================== CONFIGURACIÓN ====================
GL_LIBS="$HOME/.gldev"

GLM_INCLUDE="$GL_LIBS/glm/include"
GLM_LIB="$GL_LIBS/glm/lib"

GLAD_INCLUDE="$GL_LIBS/glad/include"
GLAD_SRC="$GL_LIBS/glad/src/glad.c"

GLFW_INCLUDE="$GL_LIBS/glfw/include"
GLFW_LIB="$GL_LIBS/glfw/lib"

IMGUI_DIR="libs/imgui"
BUILD_DIR="build"

# Crear carpeta de build si no existe
mkdir -p "$BUILD_DIR"

# ==================== COMPILAR IMGUI COMO LIBRERÍA ESTÁTICA ====================
echo "=== Compilando ImGui como librería estática ==="

# Buscar automáticamente todos los .cpp dentro de libs/imgui/
# (esto incluye imgui.cpp, imgui_draw.cpp, imgui_widgets.cpp, imgui_tables.cpp,
#  imgui_demo.cpp, imgui_impl_glfw.cpp, imgui_impl_opengl3.cpp, etc.)
mapfile -t IMGUI_CPP_FILES < <(find "$IMGUI_DIR" -name "*.cpp" -type f)

if [ ${#IMGUI_CPP_FILES[@]} -eq 0 ]; then
    echo "Error: No se encontraron archivos .cpp en $IMGUI_DIR"
    exit 1
fi

echo "Se encontraron ${#IMGUI_CPP_FILES[@]} archivos .cpp de ImGui"

# Compilar solo los archivos que hayan cambiado
for src in "${IMGUI_CPP_FILES[@]}"; do
    obj="$BUILD_DIR/$(basename "${src%.cpp}").o"
    
    if [ ! -f "$obj" ] || [ "$src" -nt "$obj" ]; then
        echo "Compilando $(basename "$src") ..."
        g++ -c "$src" -o "$obj" \
            -I "$IMGUI_DIR" \
            -I "$GLFW_INCLUDE" \
            -I "$GLAD_INCLUDE" \
            -I "$GLM_INCLUDE" \
            -O3 -std=c++17 -fPIC
    fi
done

# Crear/actualizar la librería estática
ar rcs "$BUILD_DIR/libimgui.a" "$BUILD_DIR"/*.o

# ==================== COMPILAR TU PROGRAMA ====================
echo "=== Compilando main.cpp ==="

g++ main.cpp "$GLAD_SRC" -o main \
    -I "$IMGUI_DIR" \
    -I "$GLFW_INCLUDE" \
    -I "$GLAD_INCLUDE" \
    -I "$GLM_INCLUDE" \
    -L "$BUILD_DIR" \
    -L "$GLFW_LIB" \
    -L "$GLM_LIB" \
    -limgui \
    -lglfw3 \
    -lglm \
    -lGL -ldl -lpthread \
    -O3 -std=c++17

echo "¡Compilación terminada!"
echo "Ejecuta con: ./main"