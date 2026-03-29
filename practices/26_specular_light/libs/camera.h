#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "log_vec.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

class Camera {
    public:
        Camera(){};

        void set_position( vec3 new_pos) {
            position = new_pos;
        }

        void move_forward( float sign ) {
            position += direction * sign;
        }

        void move_horizontally( float sign ) {
            position += right * sign;
        }
        
        void move_vertically( float sign ) {
            position += up * sign;
        }

        void rotate( vec3 delta_rotation ) {
            rotation += delta_rotation;
        }

        void update() {
            mat4 rot = mat4(1.0f);

            rot = glm::rotate( rot, rotation.z, vec3(0.0f, 0.0f, 1.0f) ); // Roll
            rot = glm::rotate( rot, rotation.y, vec3(0.0f, 1.0f, 0.0f) ); // Yaw
            rot = glm::rotate( rot, rotation.x, vec3(1.0f, 0.0f, 0.0f) ); // Pitch

            direction = glm::normalize( vec3( rot * vec4(0.0f,  0.0f, -1.0f, 0.0f) ) );
            right     = glm::normalize( vec3( rot * vec4(1.0f,  0.0f,  0.0f, 0.0f) ) );
            up        = glm::normalize( vec3( rot * vec4(0.0f,  1.0f,  0.0f, 0.0f) ) );
        }

        mat4 get_lookup_view() {
            return glm::lookAt( position, position + direction, up );
        }


        void print_info() {
            std::cout << "--------------------------------\n";
            std::cout << "Camera pos "      << position  << "\n"; 
            std::cout << "Camera dir "      << direction << "\n"; 
            std::cout << "Camera right "    << right     << "\n"; 
            std::cout << "Camera up "       << up        << "\n"; 
            std::cout << "Camera rotation " << rotation  << "\n"; 
        }
    public:
        vec3 position  = vec3(0.0f, 0.0f, 0.0f);
        vec3 rotation  = vec3(0.0f, 0.0f, 0.0f);    
        vec3 direction = vec3(0.0f, 0.0f, -1.0f);
        vec3 up        = vec3(0.0f, 0.0f, 0.0f);
        vec3 right     = vec3(0.0f, 0.0f, 0.0f);
};

Camera cam;

#endif