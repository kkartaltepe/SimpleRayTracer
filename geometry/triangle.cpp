#ifndef TRIANGLE_CPP
#define TRIANGLE_CPP

#include <stdio.h>
#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"
#include "plane.cpp"

class Triangle {
private:
public:
    Plane plane;


  Triangle(Plane p): plane(p) { }
  Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3): plane(p1, p2, p3) { }
  /**
   * calculate the intersection point of a ray with this plane
   * @param  ray
   */
  Intersection intersect(Ray ray){
    Intersection intersection = plane.intersect(ray);
    if(!intersection.didHit()) //It didnt hit the plane
      return intersection;

    glm::vec3 baryCoords = baryOf(intersection.point);
    if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
      //printf("Failed to hit triangle with bary (%f, %f, %f)\n", baryCoords.x,  baryCoords.y,  baryCoords.z);
      intersection.hit = false;
    } //It hit plane but didnt hit the triangle

    return intersection;
  }

  /**
   * return barycentric coordinates of an interection point
   * following http://en.wikipedia.org/wiki/Barycentric_coordinate_system
   * @param  point
   * @return
   */
  glm::vec3 baryOf(glm::vec3 point) {
    glm::vec3 edge1,edge2,p1ToPoint,p2ToPoint,p3ToPoint,baryCoords;
    edge1 = plane.p2 - plane.p1;
    edge2 = plane.p3 - plane.p1;
    p1ToPoint = point - plane.p1;

    //IT FINALLY WORKS THANKS TO http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
    //Literally everywhere on the internet has it wrong.
    float denom = glm::dot(edge1, edge1)*glm::dot(edge2,edge2)-pow(glm::dot(edge1, edge2),2);
    float uNumer = glm::dot(edge2, edge2)*glm::dot(p1ToPoint, edge1)-glm::dot(edge1, edge2)*glm::dot(p1ToPoint, edge2);
    float vNumer = glm::dot(edge1, edge1)*glm::dot(p1ToPoint, edge2)-glm::dot(edge1, edge2)*glm::dot(p1ToPoint, edge1);

    baryCoords.x = uNumer/denom;
    baryCoords.y = vNumer/denom;
    baryCoords.z = 1-baryCoords.x-baryCoords.y;
    return baryCoords;
  }
};

#endif /* end of include guard: TRIANGLE_CPP */
