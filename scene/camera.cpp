#ifndef CAMERA_CPP
#define CAMERA_CPP
#define GLM_FORCE_RADIANS

#include <vector>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../geometry/ray.hpp"

class Camera {
private:
  glm::dvec3 location, target, up;
  float fieldOfView;
  int raysWide,raysHigh;
  std::vector<Ray> rays;

public:
  Camera(glm::dvec3 location, glm::dvec3 target, glm::dvec3 up, float fieldOfViewDegrees, int width, int height)
    : location(location), target(target), up(up), raysWide(width), raysHigh(height){ fieldOfView = fieldOfViewDegrees*3.14159/180; }

  std::vector<Ray> raysToCast() {
    rays.clear();
    double pixelWidth = 2.0f*tanf(fieldOfView/2.0f)/raysWide; // using a plane 1 unit from the camera.
    double pixelHeight = pixelWidth*raysHigh/raysWide; // square out our pixels
    glm::dvec3 direction = glm::normalize(target - location);
    glm::dvec3 codirection = glm::normalize(glm::cross(direction, up)); //Right
    glm::dvec3 orthoUp = glm::normalize(glm::cross(codirection, direction));
    glm::dvec3 topLeftDirection = direction - raysWide/2.0f*pixelWidth*codirection + raysHigh/2.0f*pixelHeight*orthoUp;
    for(int y = 0; y < raysHigh; y++) {
      for(int x = 0; x < raysWide; x++) {
        glm::dvec3 rayDirection = topLeftDirection + x*pixelWidth*codirection - y*pixelHeight*orthoUp;
        rays.push_back(Ray(location, rayDirection));
      }
    }
    return rays;
  }
};

#endif /* end of include guard: CAMERA_CPP */
