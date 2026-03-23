#ifndef LOG_VEC_H
#define LOG_VEC_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

std::ostream& operator<<( std::ostream& out, const vec2& input  ) {
    return out << "x: " << input.x << ", y: " << input.y;
}


std::ostream& operator<<( std::ostream& out, const vec3& input  ) {
    return out << "x: " << input.x << ", y: " << input.y << ", z: " << input.z;
}

#endif // LOG_VEC_H