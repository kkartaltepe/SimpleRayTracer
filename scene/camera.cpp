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
  glm::vec3 location, target, up;
  float fieldOfView;
  int raysWide,raysHigh;
  std::vector<Ray> rays;

public:
  Camera(glm::vec3 location, glm::vec3 target, glm::vec3 up, float fieldOfViewDegrees, int width, int height)
    : location(location), target(target), up(up), raysWide(width), raysHigh(height){ fieldOfView = fieldOfViewDegrees*3.14159/180; }

  std::vector<Ray> raysToCast() {
    rays.clear();
    float pixelWidth = 2.0f*tanf(fieldOfView/2.0f)/raysWide; // using a plane 1 unit from the camera.
    float pixelHeight = pixelWidth*raysHigh/raysWide; // square out our pixels
    glm::vec3 direction = glm::normalize(target - location);
    glm::vec3 codirection = glm::normalize(glm::cross(direction, up)); //Right
    glm::vec3 orthoUp = glm::normalize(glm::cross(codirection, direction));
    glm::vec3 topLeftDirection = direction - raysWide/2.0f*pixelWidth*codirection + raysHigh/2.0f*pixelHeight*orthoUp;
    for(int y = 0; y < raysHigh; y++) {
      for(int x = 0; x < raysWide; x++) {
        glm::vec3 rayDirection = topLeftDirection + x*pixelWidth*codirection - y*pixelHeight*orthoUp;
        rays.push_back(Ray(location, rayDirection));
      }
    }
    return rays;
  }
};

#endif /* end of include guard: CAMERA_CPP */
