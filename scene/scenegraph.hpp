#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <map>

#include "../display/simple.hpp"
#include "object.hpp"
#include "material.hpp"
#include "light.hpp"
#include "camera.cpp"

class SceneGraph {
public:
  std::vector<Light> lights;
  std::vector<Object> objects;
  std::map<std::string, Material> materials;

  Camera sceneCamera;

  SceneGraph(): sceneCamera(glm::vec3(0.0f, 0.0f, -1.0f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f),
                            90.0f, PROJ_WIDTH, PROJ_HEIGHT) { };
  void addObject(Object object){
    objects.push_back(object);
  }

  void generateBvhs() {
    for(std::vector<Object>::iterator iter = objects.begin(); iter != objects.end(); iter++)
      iter->generateBvh();
  }
  void addLight(Light light) {
    lights.push_back(light);
  }
  void addMaterial(Material material) {
    materials[material.name] = material;
  }
  Material getMaterial(std::string name) {
    return materials[name];
  }
  void setCamera(Camera camera) {
    sceneCamera = camera;
  }
};

#endif /* end of include guard: SCENEGRAPH_HPP */
