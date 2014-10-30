#include <glm/glm.hpp>
#include <math.h>

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
  Intersection intersect(Ray ray){
    glm::vec3 originToCenter = center - ray.origin;
    float projectionLength = glm::dot(ray.direction, originToCenter);
    float distToCenterSquared = glm::dot(originToCenter, originToCenter);
    float distAlongProjSquared = projectionLength*projectionLength;
    float discriminant = distAlongProjSquared - distToCenterSquared + radius*radius;
    if(discriminant < 0)
      return Intersection();
    else {
      //Find the closest intersection point by ray*d = quadratic formula
      float distanceAlongRayPlus = sqrtf(discriminant) - projectionLength;
      float distanceAlongRayMinus = -1.0f * sqrtf(discriminant) - projectionLength;
      glm::vec3 intersectionPoint = ray.direction * fminf(distanceAlongRayPlus, distanceAlongRayMinus);
      return Intersection(intersectionPoint, ray, glm::normalize(intersectionPoint-center));
    }
  }
};
