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
    if(discriminant < 0.0f)
      return Intersection();
    else {
      //Find the closest intersection point by ray*d = quadratic formula
      float distanceAlongRayPlus = (-1.0f*projectionLength) + sqrtf(discriminant);
      float distanceAlongRayMinus = (-1.0f*projectionLength) - sqrtf(discriminant);
      bool inside = distToCenterSquared <= radius*radius-0.00001f;
      float t = 0.0f;

      if(inside){ // The ray starts inside the circle
        t = fmaxf(distanceAlongRayPlus, distanceAlongRayMinus); // One of these will be negative
      } else {
        if(distanceAlongRayPlus < 0.00005f) // ray hit too close to consider
          t = distanceAlongRayMinus;
        else if(distanceAlongRayMinus < 0.00005f)
          t = distanceAlongRayPlus;
        else
          t = fminf(distanceAlongRayPlus, distanceAlongRayMinus);
      }

      if(t < 0.00005f) // The hit was behind the origin of the ray.
        return Intersection();

      glm::vec3 intersectionPoint = ray.direction*t+ray.origin;
      // printf("IntersectionPoint (%f, %f, %f)\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
      glm::vec3 normal = glm::normalize(intersectionPoint-center);
      // Regularize the intersection point ala http://www.cse.yorku.ca/~amana/research/regularization.pdf and shift it just outside the circle.
      return Intersection(intersectionPoint, ray, fabs(t), normal, inside);
    }
  }
};

#endif /* end of include guard: CIRCLE_CPP */
