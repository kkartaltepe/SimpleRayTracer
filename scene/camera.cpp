#ifndef CAMERA_CPP
#define CAMERA_CPP
#define GLM_FORCE_RADIANS

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../geometry/ray.hpp"

class Camera {
private:
  glm::vec3 location, target, up;
  float fieldOfView;
  int raysWide,raysHigh;

public:
  Camera(glm::vec3 origin, glm::vec3 target, glm::vec3 up, float fieldOfView, int width, int height)
    : location(origin), target(target), up(up), fieldOfView(fieldOfView), raysWide(width), raysHigh(height){ }

  std::vector<Ray> raysToCast() {
    std::vector<Ray> rays;
    //float dTheta = fieldOfView/3.1459*180/raysWide; //Thetas per pixel(ray)
    float pixelWidth = 2.0f*tanf(fieldOfView*3.14159/180.0f/2.0f)/raysWide;
    float pixelHeight = pixelWidth; //Square pixels
    glm::vec3 direction = glm::normalize(target - location);
    glm::vec3 codirection = glm::normalize(glm::cross(up,direction));
    glm::vec3 orthoUp = glm::normalize(glm::cross(codirection, direction));
    glm::vec3 topLeftRay = direction - raysWide/2.0f*pixelWidth*codirection + raysHigh/2.0f*pixelHeight*orthoUp;
    for(int y = 0; y < raysHigh; y++) {
      for(int x = 0; x < raysWide; x++) {
        glm::vec3 ray = topLeftRay + codirection*pixelWidth*(float)x - orthoUp*pixelHeight*(float)y;
        rays.push_back(Ray(location, ray));
      }
    }
    return rays;
  }
};

#endif /* end of include guard: CAMERA_CPP */
