#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP
#include <glm/glm.hpp>

#include "ray.hpp"

class Intersection {
public:
  glm::vec3 point;
  Ray incident;
  glm::vec3 normal;
  bool hit;
  //and a Material?
  Intersection(glm::vec3 p, Ray i, glm::vec3 n)
    : point(p), incident(i), normal(n) { hit=true; }
  Intersection() { hit=false; }
  bool didHit() { return hit;}
};

#endif /* end of include guard: INTERSECTION_HPP */
