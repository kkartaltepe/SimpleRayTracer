#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

class Ray {
public:
  glm::vec3 origin;
  glm::vec3 direction;
  Ray(glm::vec3 origin, glm::vec3 dir)
    : origin(origin) {direction = glm::normalize(dir);}
};

#endif /* end of include guard: RAY_HPP */
