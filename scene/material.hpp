#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>
#include <string>

class Material {
public:
  std::string name;
  glm::vec3 dColor, sColor, aColor, rColor;
  float reflectivity, refractiveIndex, opacity;
  int specHardness;
  Material(){
    dColor = glm::vec3(1.0f, 1.0f, 1.0f);
    reflectivity = 0;
    specHardness = 0;
    refractiveIndex = 1.0;
    opacity = 1.0;
  }
  Material(std::string name): name(name){
    reflectivity = 0;
    specHardness = 0;
    refractiveIndex = 1.0;
    opacity = 1.0;
  }
  Material(std::string name, glm::vec3 dColor, glm::vec3 sColor, glm::vec3 aColor, float _specHardness, float _refractiveIndex, int _opacity):
    name(name), dColor(dColor), sColor(sColor), aColor(aColor){
      specHardness = _specHardness;
      refractiveIndex = _refractiveIndex;
      opacity = _opacity;
  }
};

#endif /* end of include guard: MATERIAL_HPP */
