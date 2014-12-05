#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>
#include <string>

class Material {
public:
  std::string name;
  glm::vec3 dColor, sColor, aColor;
  float refractiveIndex, opacity;
  int specHardness;
  Material(){
    specHardness = 0;
    refractiveIndex = 0;
    opacity = 0;
  }
  Material(std::string name): name(name){
    specHardness = 0;
    refractiveIndex = 0;
    opacity = 0;
  }
  Material(std::string name, glm::vec3 dColor, glm::vec3 sColor, glm::vec3 aColor, float _specHardness, float _refractiveIndex, int _opacity):
    name(name), dColor(dColor), sColor(sColor), aColor(aColor){
      specHardness = _specHardness;
      refractiveIndex = _refractiveIndex;
      opacity = _opacity;
  }
};

#endif /* end of include guard: MATERIAL_HPP */
