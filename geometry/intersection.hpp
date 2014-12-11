#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP
#include <glm/glm.hpp>

#include "ray.hpp"

class Object; //forward declaration of object to prevent circular inclusions

class Intersection {
public:
  glm::vec3 point;
  Ray incident;
  float distanceTraveled;
  glm::vec3 normal;
  Object* object;
  bool hit;
  //and a Material?
  Intersection(glm::vec3 p, Ray i, float distance, glm::vec3 n)
    : point(p), incident(i), distanceTraveled(distance), normal(n) { hit=true; }
  Intersection() { hit=false; }
  bool didHit() { return hit;}
};

#endif /* end of include guard: INTERSECTION_HPP */
