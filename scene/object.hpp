#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <vector>

#include "../geometry/circle.cpp"
#include "../geometry/triangle.cpp"
#include "material.hpp"

class Object {
public:
  std::vector<Triangle> triangles;
  std::vector<Circle> circles;
  Material material;
  std::string name;

  Object() { }
  Object(std::string name): name(name) { }
};

#endif /* end of include guard: OBJECT_HPP */
