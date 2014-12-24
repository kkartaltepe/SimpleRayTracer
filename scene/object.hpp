#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <vector>

#include "../geometry/circle.cpp"
#include "../geometry/triangle.cpp"
#include "../geometry/boundTree.cpp"
#include "material.hpp"

class Object {
public:
  std::vector<Triangle> triangles;
  std::vector<Circle> circles;
  BoundTree bvh;

  Material material;
  std::string name;

  Object() { }
  Object(std::string name): name(name) { }

  void generateBvh() {
  	std::vector<Triangle*> trianglePtrs;
  	for(std::vector<Triangle>::iterator iter = triangles.begin(); iter != triangles.end(); iter++)
  		trianglePtrs.push_back(&*iter);

  	bvh = BoundTree(trianglePtrs);
  }
};


#endif /* end of include guard: OBJECT_HPP */
