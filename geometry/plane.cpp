#ifndef PLANE_CPP
#define PLANE_CPP

#include <cstdio>
#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"

class Plane {
public:
  glm::vec3 p1,p2,p3,normal;
  Plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3): p1(p1), p2(p2), p3(p3) {
    normal = glm::normalize(glm::cross(p2-p1, p3-p2));
  }
  /**
   * calculate the intersection point of a ray with this plane, the plane is
   * is assumed to be defined by a clockwise winding of points.
   * @param  ray
   */
  Intersection intersect(Ray ray) const{
    float numerator = -1.0f*(glm::dot(ray.origin, normal) - glm::dot(normal, p2)); //Times -1 to ensure clockwise windings give correct t value
    float denominator = glm::dot(ray.direction, normal);


    if(denominator == 0) { //Ray is perpendicular to the normal, does not intersect the plane unless it lays within the plane.
      return Intersection();
    } else { //The ray intersects the plane, at numerator/denominator*ray.
      float t = numerator/denominator;
      glm::vec3 intersectionPoint = ray.direction*t+ ray.origin;
      if(t > 0)
        return Intersection(intersectionPoint+(normal*0.000001f), ray, fabs(t), normal);
      else
        return Intersection();
    }
  }
};


#endif /* end of include guard: PLANE_CPP */
