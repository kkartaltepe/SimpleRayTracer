#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include "../display/simple.hpp"
#include "../geometry/circle.cpp"
#include "../geometry/triangle.cpp"
#include "../geometry/loader.cpp"
#include "light.hpp"
#include "camera.cpp"

class SceneGraph {
public:
  std::vector<Circle> circles;
  std::vector<Triangle> triangles;
  std::vector<Light> lights;
  Camera sceneCamera;

  SceneGraph(): sceneCamera(glm::vec3(0.0f, 0.0f, -1.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            90.0f, PROJ_WIDTH, PROJ_HEIGHT) { };

  void load(const char * sceneDataPath) {
    triangles = loadTriangles(sceneDataPath);
  };
  void addCircle(Circle circle){
    circles.push_back(circle);
  }
  void addTriangle(Triangle triangle){
    triangles.push_back(triangle);
  }
  void addLight(Light light) {
    lights.push_back(light);
  };
  void setCamera(Camera camera) {
    sceneCamera = camera;
  };
};

#endif /* end of include guard: SCENEGRAPH_HPP */
