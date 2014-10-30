#include <stdio.h>
#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"
#include "plane.cpp"

class Triangle {
private:
    Plane plane;

public:
  Triangle(Plane p): plane(p) { }
  Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    plane = Plane(p1, p2, p3);
  }
  /**
   * calculate the intersection point of a ray with this plane
   * @param  ray
   */
  Intersection intersect(Ray ray){
    // printf("Checking if ray intersects triangle\n");
    Intersection intersection = plane.intersect(ray);
    if(!intersection.hit()) {
      // printf("Ray didnt hit plane\n");
      return Intersection(); //Didnt hit the plane
    }


    glm::vec3 baryCoords = baryOf(intersection.point);
    // printf("Hit plane at (%f,%f,%f)\n", intersection.point.x, intersection.point.y, intersection.point.z);
    // printf("Hit plane with barycentric (%f,%f,%f)\n", baryCoords.x, baryCoords.y, baryCoords.z);
    if(baryCoords.x > 0 && baryCoords.y > 0 && baryCoords.z > 0) //It hit the triangle
      return intersection;
    else
      return Intersection(); //It didnt
  }

  /**
   * return barycentric coordinates of an interection point
   * following http://en.wikipedia.org/wiki/Barycentric_coordinate_system
   * @param  point
   * @return
   */
  glm::vec3 baryOf(glm::vec3 point) {
    glm::vec3 baryCoords = glm::vec3(0.0f);
    baryCoords.x =
    ((plane.p2.y-plane.p3.y)*(point.x-plane.p3.x)+(plane.p3.x-plane.p2.x)*(point.y-plane.p3.y))
    /((plane.p2.y-plane.p3.y)*(plane.p1.x-plane.p3.x)+(plane.p3.x-plane.p2.x)*(plane.p1.y-plane.p3.y));
    baryCoords.y =
    ((plane.p3.y-plane.p1.y)*(point.x-plane.p3.x)+(plane.p1.x-plane.p3.x)*(point.y-plane.p3.y))
    /((plane.p2.y-plane.p3.y)*(plane.p1.x-plane.p3.x)+(plane.p3.x-plane.p2.x)*(plane.p1.y-plane.p3.y));
    baryCoords.z = 1-baryCoords.x-baryCoords.y;
    return baryCoords;
  }
};