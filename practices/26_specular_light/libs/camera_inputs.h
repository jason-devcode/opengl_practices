#ifndef CAMERA_INPUTS_H
#define CAMERA_INPUTS_H

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

#include "camera.h"

double last_cursor_x = 0.0;
double last_cursor_y = 0.0;
bool first_cursor_pos_update = true;

bool isFreeCameraMode = false;

void mouse_callback( GLFWwindow* window, double xpos, double ypos ) {
  if( !isFreeCameraMode ) return;

  if( first_cursor_pos_update ) {
    last_cursor_x = xpos;
    last_cursor_y = ypos;
    first_cursor_pos_update = false;
  }

  float sensitivity = 0.002f;
  float offsetx = static_cast<float>(xpos - last_cursor_x) * sensitivity;
  float offsety = static_cast<float>(ypos - last_cursor_y) * sensitivity;

  cam.rotate( vec3( -offsety, -offsetx, 0.0f ) );
  cam.update();

  last_cursor_x = xpos;
  last_cursor_y = ypos;
}

static bool spacePressedLastFrame = false;

void process_camera_inputs( GLFWwindow* window, float delta_time ) {
  float cameraSpeed = 30.0f;
  float cameraAngularSpeed = 2.0f;

  if( glfwGetKey( window, GLFW_KEY_W ) ) {
    cam.move_forward( delta_time * cameraSpeed );
    cam.update();
  }

  if( glfwGetKey( window, GLFW_KEY_S ) ) {
    cam.move_forward( -delta_time * cameraSpeed );
    cam.update();
  }

  if( glfwGetKey( window, GLFW_KEY_A ) ) {
    cam.move_horizontally( -delta_time * cameraSpeed );
    cam.update();
  }

  if( glfwGetKey( window, GLFW_KEY_D ) ) {
    cam.move_horizontally( delta_time * cameraSpeed );
    cam.update();
  }
  

  /// TOGGLE MOVE CAMERA BY CURSOR MODE
  bool spacePressed = glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS;
  
  // Solo activar cuando hay transición de NO presionado -> presionado
  if( spacePressed && !spacePressedLastFrame ) {
    isFreeCameraMode = !isFreeCameraMode;  // Corregido también el toggle

    if( isFreeCameraMode ) {
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
      first_cursor_pos_update = true;  // Reset para evitar saltos de cámara
    } else {
      glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    }
  }
  
  spacePressedLastFrame = spacePressed;
}


#endif