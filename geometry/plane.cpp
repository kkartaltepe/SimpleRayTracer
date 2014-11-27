#ifndef PLANE_CPP
#define PLANE_CPP

#include <cstdio>
#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"

class Plane {
public:
  glm::dvec3 p1,p2,p3;
  Plane(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3): p1(p1), p2(p2), p3(p3) { }
  /**
   * calculate the intersection point of a ray with this plane, the plane is
   * is assumed to be defined by a clockwise winding of points.
   * @param  ray
   */
  Intersection intersect(Ray ray){
    glm::dvec3 normal = glm::normalize(glm::cross(p2-p1, p3-p1));
    double numerator = -1.0f*(glm::dot(ray.origin, normal) + glm::dot(normal, p2)); //Times -1 to ensure clockwise windings give correct t value
    double denominator = glm::dot(ray.direction, normal);

    // printf("Testing intersection with plane composed of (%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",
    //   p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z);

    if(denominator == 0) { //Ray is perpendicular to the normal, does not intersect the plane unless it lays within the plane.
      return Intersection();
    } else { //The ray intersects the plane, at numerator/denominator*ray.
      double t = numerator/denominator;
      if(t > 0) //Infront of the ray.
        return Intersection(ray.direction*t + ray.origin, ray, normal);
      else { //Behind the ray.
        return Intersection();
      }
    }
  }
};


#endif /* end of include guard: PLANE_CPP */
