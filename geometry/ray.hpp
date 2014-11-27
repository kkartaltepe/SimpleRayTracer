#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

class Ray {
public:
  glm::dvec3 origin;
  glm::dvec3 direction;
  Ray(glm::dvec3 origin, glm::dvec3 dir)
    : origin(origin) {direction = glm::normalize(dir);}
  Ray() { }
};

#endif /* end of include guard: RAY_HPP */
