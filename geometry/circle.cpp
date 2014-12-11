#ifndef CIRCLE_CPP
#define CIRCLE_CPP

#include <glm/glm.hpp>
#include <math.h>
#include <stdio.h>

#include "intersection.hpp"
#include "ray.hpp"

class Circle {
private:
    glm::vec3 center;
    float radius;
public:
  Circle(glm::vec3 center, float radius): center(center), radius(radius) {}

  /**
   * calculate the intersection point of a ray with a circle
   * @param  ray (incoming ray)
   */
  Intersection intersect(Ray ray) const{
    glm::vec3 centerToOrigin = ray.origin - center;
    float projectionLength = glm::dot(ray.direction, centerToOrigin);
    float distToCenterSquared = glm::dot(centerToOrigin, centerToOrigin);
    float distAlongProjSquared = projectionLength*projectionLength;
    float discriminant = distAlongProjSquared - distToCenterSquared + radius*radius;
    if(discriminant < 0)
      return Intersection();
    else {
      //Find the closest intersection point by ray*d = quadratic formula
      float distanceAlongRayPlus = (-1.0f*projectionLength) + sqrtf(discriminant);
      float distanceAlongRayMinus = (-1.0f*projectionLength) - sqrtf(discriminant);
      float t;

      if(distanceAlongRayPlus < 0 && distanceAlongRayMinus < 0) //The ray starts and points outside the circle.
        return Intersection();
      if(distanceAlongRayPlus < 0 && distanceAlongRayMinus > 0) //The ray starts inside the circle!
        t = distanceAlongRayMinus;
      if(distanceAlongRayPlus > 0 && distanceAlongRayMinus < 0) //The ray starts inside the circle!
        t = distanceAlongRayPlus;
      if(distanceAlongRayPlus > 0 && distanceAlongRayMinus > 0) //The ray starts outside the circle but intersects twice.
        t = fminf(distanceAlongRayPlus, distanceAlongRayMinus);

      glm::vec3 intersectionPoint = ray.direction*t+ray.origin;
      // printf("IntersectionPoint (%f, %f, %f)\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
      glm::vec3 normal = glm::normalize(intersectionPoint-center);
      // Regularize the intersection point ala http://www.cse.yorku.ca/~amana/research/regularization.pdf and shift it just outside the circle.
      return Intersection(center+normal*(radius+0.000001f), ray, fabs(t), normal);
    }
  }
};

#endif /* end of include guard: CIRCLE_CPP */
