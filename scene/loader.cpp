#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include "scenegraph.hpp"

std::vector<std::string> splitString(std::string input, std::string delimiter);


void loadMaterials(std::string mtlFileName, SceneGraph *currentScene) {
  std::ifstream mtlDataStream(mtlFileName.c_str(), std::ios::in);

  if(mtlDataStream.is_open()) {
    std::string line = "";
    Material* curMaterial = NULL;

    while(getline(mtlDataStream, line)){
      if(line.length() < 1)
        continue;
      std::vector<std::string> values = splitString(line, std::string(" "));
      if(values[0] == "newmtl") { // Begin Definition of a new material
        if(!curMaterial)
          curMaterial = new Material(values[1]);
        else{
          currentScene->addMaterial(*curMaterial);
          *curMaterial = Material(values[1]);
        }
      }
      if(values[0] == "Ns") { // Specular Hardness
        curMaterial->specHardness = atof(values[1].c_str());
      }
      if(values[0] == "Ka") { // Ambient Component
        curMaterial->aColor = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
      }
      if(values[0] == "Kd") { // Diffuse Component
        curMaterial->dColor = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
      }
      if(values[0] == "Ks") { // Specular Component
        curMaterial->sColor = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
      }
      if(values[0] == "d") { // Opacity
        curMaterial->opacity = atof(values[1].c_str());
      }
      if(values[0] == "Ni") { // Index of refraction
        curMaterial->refractiveIndex = atof(values[1].c_str());
      }
    }
    if(curMaterial)
      currentScene->addMaterial(*curMaterial);
    delete curMaterial;
  } else {
    printf("Failed to open material data at %s\n", mtlFileName.c_str());
  }

  mtlDataStream.close();
}

/**
 * Load mesh data from a Wavefront OBJ file. (exported from blender)
 * Dont look at me im HIDEOUS
 */
SceneGraph loadScene(std::string sceneDataPath) {
  std::vector<glm::vec3> vertices;
  Object* curObject = NULL;
  SceneGraph scene;
  std::ifstream sceneDataStream(sceneDataPath.c_str(), std::ios::in);

  if(sceneDataStream.is_open()) {
    std::string line = "";
    while(getline(sceneDataStream, line)){
      if(line.length() < 1)
        continue;
      std::vector<std::string> values = splitString(line, std::string(" "));
      if(values[0] == "mtllib") { //Load a some materials
        std::string currentDirectory = sceneDataPath.substr(0, sceneDataPath.find_last_of("/\\")+1);
        loadMaterials(currentDirectory+values[1], &scene);
      }
      if(values[0] == "l") { //Light
        glm::vec3 location = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
        glm::vec3 color = glm::vec3(atof(values[4].c_str()), atof(values[5].c_str()), atof(values[6].c_str()));
        scene.addLight(Light(location, color));
      }
      if(values[0] == "r") { //cameRa
        glm::vec3 location = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
        glm::vec3 target = glm::vec3(atof(values[4].c_str()), atof(values[5].c_str()), atof(values[6].c_str()));
        glm::vec3 up = glm::vec3(atof(values[7].c_str()), atof(values[8].c_str()), atof(values[9].c_str()));
        float fov = atof(values[10].c_str());
        int raysWide = atof(values[11].c_str()), raysHigh = atof(values[12].c_str());
        scene.setCamera(Camera(location, target, up, fov, raysWide, raysHigh));
      }
      if(values[0] == "o") {// Describes a new object.
        if(!curObject)
          curObject = new Object(values[1]);
          else{
            scene.addObject(*curObject);
            *curObject = Object(values[1]);
          }
      }
      if(values[0] == "usemtl") { // set the material for the current object.
        curObject->material = scene.getMaterial(values[1]);
      }
      if(values[0] == "v") {//Describes a vertex
        float first = atof(values[1].c_str()), second = atof(values[2].c_str()), third = atof(values[3].c_str());
        glm::vec3 vertex = glm::vec3(first, second, third);
        vertices.push_back(vertex);
      }
      if(values[0] == "f") {//Describes a face, follows after vertices in file.
        int first = atoi(values[1].c_str()), second = atoi(values[2].c_str()), third = atoi(values[3].c_str());
        curObject->triangles.push_back(Triangle(vertices[first-1], vertices[second-1], vertices[third-1]));
      }
      if(values[0] == "c") { //Circle
        glm::vec3 location = glm::vec3(atof(values[1].c_str()), atof(values[2].c_str()), atof(values[3].c_str()));
        float radius = atof(values[4].c_str());
        curObject->circles.push_back(Circle(location, radius));
      }
    }
    sceneDataStream.close();
  } else { printf("Failed to load mesh data from '%s'\n", sceneDataPath.c_str()); }
  if(curObject)
    scene.addObject(*curObject);
    delete curObject;
  return scene;
}

std::vector<std::string> splitString(std::string input, std::string delimiter) {
  std::vector<std::string> values;
  int prevPosition = -1;
  int position = input.find(delimiter);
  while( position != std::string::npos) {
    std::string value = input.substr(prevPosition+1, position - prevPosition - 1);
    values.push_back(value);
    prevPosition = position;
    position = input.find(delimiter, prevPosition+1);
  }
  values.push_back(input.substr(prevPosition+1)); //add remainder of the data
  return values;
}
