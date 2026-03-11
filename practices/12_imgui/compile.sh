#!/usr/bin/env bash
set -euo pipefail  # fail fast on errors

# ─── Configuration ────────────────────────────────────────────────
IMGUI_DIR="./libs/imgui"
GLAD_DIR="$HOME/.gldev/glad"
GLFW_DIR="$HOME/.gldev/glfw"

# ImGui backend + core files (add more .cpp files if you use them)
IMGUI_SOURCES=(
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)

GLAD_SOURCE="${GLAD_DIR}/src/glad.c"

# ─── Compile ──────────────────────────────────────────────────────
g++ -std=c++17 -Wall -Wextra -O2 \
    main.cpp \
    "${GLAD_SOURCE}" \
    "${IMGUI_SOURCES[@]}" \
    -o main \
    -I "${GLAD_DIR}/include" \
    -I "${GLFW_DIR}/include" \
    -I "${IMGUI_DIR}" \
    -L "${GLFW_DIR}/lib" \
    -lglfw3 \
    -lGL -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lc

# Optional: more portable linux linking (especially static glfw)
# -lGL -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi