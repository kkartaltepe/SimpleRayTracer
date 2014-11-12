#ifndef PLANE_CPP
#define PLANE_CPP

#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"

class Plane {
public:
  glm::vec3 p1,p2,p3;
  Plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3): p1(p1), p2(p2), p3(p3) { }
  /**
   * calculate the intersection point of a ray with this plane
   * @param  ray
   */
  Intersection intersect(Ray ray){
    glm::vec3 originToPlane = p1 - ray.origin;//- ray->origin // Should be expanded to include rays not beginning at the origin.
    glm::vec3 normal = glm::normalize(glm::cross(p2-p1, p3-p1));
    float numerator = glm::dot(originToPlane, normal);
    float denominator = glm::dot(ray.direction, normal);

    if(denominator == 0) { //Ray is perpendicular to the normal, does not intersect the plane unless it lays within the plane.
      return Intersection();
    } else { //The ray intersects the plane, at numerator/denominator*ray.
      return Intersection(ray.direction*numerator/denominator - ray.origin, ray, normal);
    }
  }
};


#endif /* end of include guard: PLANE_CPP */
