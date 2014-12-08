#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

class Light {
public:
  glm::vec3 location;
  glm::vec3 color; // All components should be from 0-1.0f.
  float power;
  Light(glm::vec3 location, glm::vec3 color, float power)
    : location(location), color(color), power(power) { }
  Light() { }
};

#endif /* end of include guard: LIGHT_HPP */
