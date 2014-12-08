#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

class Light {
public:
  glm::vec3 location;
  glm::vec3 color;
  Light(glm::vec3 location, glm::vec3 color)
    : location(location), color(color) { }
  Light() { }
};

#endif /* end of include guard: LIGHT_HPP */
