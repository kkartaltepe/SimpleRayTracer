#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

class Light {
public:
  glm::vec3 location;
  glm::vec3 difColor, specColor;
  Light(glm::vec3 location, glm::vec3 color)
    : location(location), difColor(color), specColor(glm::vec3(0.0f)) { }
    Light(glm::vec3 location, glm::vec3 diffuse, glm::vec3 specular)
    : location(location), difColor(diffuse), specColor(specular) { }
  Light() { }
};

#endif /* end of include guard: LIGHT_HPP */
