#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

class Ray {
public:
  glm::vec3 origin;
  glm::vec3 direction;
  glm::vec3 invDirection;
  Ray(glm::vec3 origin, glm::vec3 dir)
    : origin(origin) {direction = glm::normalize(dir);
    				  invDirection = glm::vec3(1.0f)/direction;}
  Ray() { }
};

#endif /* end of include guard: RAY_HPP */
