#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include "triangle.cpp"

/**
 * Load mesh data from a Wavefront OBJ file. (exported from blender)
 * Dont look at me im HIDEOUS
 */
std::vector<Triangle> loadTriangles(const char * meshDataPath) {
  std::vector<glm::vec3> vertices;
  std::vector<Triangle> triangles;
  std::ifstream mestDataStream(meshDataPath, std::ios::in);

  if(mestDataStream.is_open()){
    std::string line = "";
    while(getline(mestDataStream, line)){
      if(line.length() < 1 || line[0] == '#')
        continue;
      if(line[0] == 'v') {//Describes a vertex
        std::string::size_type first=2, second, third; //first space follows immediately from 'v '
        float firstF, secondF, thirdF;
        second = line.find_first_of(std::string(" "), first)+1; //skip the space we find
        third = line.find_first_of(std::string(" "), second)+1;

        firstF = atof(line.substr(first, second-first).c_str());
        secondF = atof(line.substr(second, third-second).c_str());
        thirdF = atof(line.substr(third, line.length()-third).c_str());
        glm::vec3 vertex = glm::vec3(firstF, secondF, thirdF);
        vertices.push_back(vertex);
      }
      else if(line[0] == 'f') {//Describes a face, follows after vertices in file.
        std::string::size_type first=2, second, third; //first space follows immediately from 'f '
        int firstI, secondI, thirdI;
        second = line.find_first_of(std::string(" "), first)+1;
        third = line.find_first_of(std::string(" "), second)+1;

        firstI = atoi(line.substr(first, second-first).c_str())-1;  //Verticies in the file are 1-indexed
        secondI = atoi(line.substr(second, third-second).c_str())-1;
        thirdI = atoi(line.substr(third, line.length()-third).c_str())-1;
        triangles.push_back(Triangle(vertices[firstI], vertices[secondI], vertices[thirdI]));
      }
    }
    mestDataStream.close();
  } else { printf("Failed to load mesh data\n"); }
  return triangles;
}
