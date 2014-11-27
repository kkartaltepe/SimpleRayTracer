#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

class Light {
public:
  glm::dvec3 location;
  glm::dvec3 color;
  Light(glm::dvec3 location, glm::dvec3 color)
    : location(location), color(color) { }
  Light() { }
};

#endif /* end of include guard: LIGHT_HPP */
