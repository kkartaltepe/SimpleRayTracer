#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include "scenegraph.hpp"

std::vector<std::string> splitString(std::string input, std::string delimiter);

/**
 * Load mesh data from a Wavefront OBJ file. (exported from blender)
 * Dont look at me im HIDEOUS
 */
SceneGraph loadScene(const char * sceneDataPath) {
  std::vector<glm::vec3> vertices;
  SceneGraph scene;
  std::ifstream sceneDataStream(sceneDataPath, std::ios::in);

  if(sceneDataStream.is_open()){
    std::string line = "";
    while(getline(sceneDataStream, line)){
      if(line.length() < 1 || line[0] == '#')
        continue;
      if(line[0] == 'l') { //Light
        std::vector<std::string> values = splitString(line.substr(2), std::string(" "));
        glm::vec3 location = glm::vec3(atof(values[0].c_str()), atof(values[1].c_str()), atof(values[2].c_str()));
        glm::vec3 difColor = glm::vec3(atof(values[3].c_str()), atof(values[4].c_str()), atof(values[5].c_str()));
        if(values.size() < 9) {
          scene.addLight(Light(location, difColor));
          continue;
        }
        glm::vec3 specColor = glm::vec3(atof(values[6].c_str()), atof(values[7].c_str()), atof(values[8].c_str()));
        scene.addLight(Light(location, difColor, specColor));
      }
      if(line[0] == 'c') { //Circle
        std::vector<std::string> values = splitString(line.substr(2), std::string(" "));
        glm::vec3 location = glm::vec3(atof(values[0].c_str()), atof(values[1].c_str()), atof(values[2].c_str()));
        float radius = atof(values[3].c_str());
        scene.addCircle(Circle(location, radius));
      }
      if(line[0] == 'r') { //cameRa
        std::vector<std::string> values = splitString(line.substr(2), std::string(" "));
        glm::vec3 location = glm::vec3(atof(values[0].c_str()), atof(values[1].c_str()), atof(values[2].c_str()));
        glm::vec3 target = glm::vec3(atof(values[3].c_str()), atof(values[4].c_str()), atof(values[5].c_str()));
        glm::vec3 up = glm::vec3(atof(values[6].c_str()), atof(values[7].c_str()), atof(values[8].c_str()));
        float fov = atof(values[9].c_str());
        int raysWide = atof(values[10].c_str()), raysHigh = atof(values[11].c_str());
        scene.setCamera(Camera(location, target, up, fov, raysWide, raysHigh));
      }
      if(line[0] == 'v') {//Describes a vertex
        std::vector<std::string> values = splitString(line.substr(2), std::string(" "));
        float first = atof(values[0].c_str()), second = atof(values[1].c_str()), third = atof(values[2].c_str());
        glm::vec3 vertex = glm::vec3(first, second, third);
        vertices.push_back(vertex);
      }
      else if(line[0] == 'f') {//Describes a face, follows after vertices in file.
        std::vector<std::string> values = splitString(line.substr(2), std::string(" "));
        int first = atoi(values[0].c_str()), second = atoi(values[1].c_str()), third = atoi(values[2].c_str());
        scene.addTriangle(Triangle(vertices[first-1], vertices[second-1], vertices[third-1]));
      }
    }
    sceneDataStream.close();
  } else { printf("Failed to load mesh data from '%s'\n", sceneDataPath); }
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
