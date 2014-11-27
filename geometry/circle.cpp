#ifndef CIRCLE_CPP
#define CIRCLE_CPP

#include <glm/glm.hpp>
#include <math.h>

#include "intersection.hpp"
#include "ray.hpp"

class Circle {
private:
    glm::dvec3 center;
    double radius;
public:
  Circle(glm::dvec3 center, double radius): center(center), radius(radius) {}

  /**
   * calculate the intersection point of a ray with a circle
   * @param  ray (incoming ray)
   */
  Intersection intersect(Ray ray){
    glm::dvec3 originToCenter = center - ray.origin;
    double projectionLength = glm::dot(ray.direction, originToCenter);
    double distToCenterSquared = glm::dot(originToCenter, originToCenter);
    double distAlongProjSquared = projectionLength*projectionLength;
    double discriminant = distAlongProjSquared - distToCenterSquared + radius*radius;
    if(discriminant < 0)
      return Intersection();
    else {
      //Find the closest intersection point by ray*d = quadratic formula
      double distanceAlongRayPlus = sqrt(discriminant) - projectionLength;
      double distanceAlongRayMinus = -1.0f * sqrt(discriminant) - projectionLength;
      glm::dvec3 intersectionPoint = ray.direction * fmin(distanceAlongRayPlus, distanceAlongRayMinus);
      return Intersection(intersectionPoint, ray, glm::normalize(intersectionPoint-center));
    }
  }
};

#endif /* end of include guard: CIRCLE_CPP */
