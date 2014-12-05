#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <map>

#include "../display/simple.hpp"
#include "../geometry/circle.cpp"
#include "../geometry/triangle.cpp"
#include "material.hpp"
#include "light.hpp"
#include "camera.cpp"

class SceneGraph {
public:
  std::vector<Circle> circles;
  std::vector<Triangle> triangles;
  std::vector<Light> lights;
  std::map<std::string, Material> materials;
  //TODO: Abstract away objects in the scene from their geometry.
  
  Camera sceneCamera;

  SceneGraph(): sceneCamera(glm::vec3(0.0f, 0.0f, -1.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            90.0f, PROJ_WIDTH, PROJ_HEIGHT) { };
  void addCircle(Circle circle,  std::string materialName = "None"){
    circles.push_back(circle);
  }
  void addTriangle(Triangle triangle, std::string materialName = "None"){
    triangles.push_back(triangle);
  }
  void addLight(Light light) {
    lights.push_back(light);
  };
  void addMaterial(Material material) {
    materials[material.name] = material;
  }
  void setCamera(Camera camera) {
    sceneCamera = camera;
  };
};

#endif /* end of include guard: SCENEGRAPH_HPP */
