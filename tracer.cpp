#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>


#include "display/simple.hpp"
#include "geometry/circle.cpp"
#include "geometry/triangle.cpp"
#include "geometry/ray.hpp"
#include "scene/light.hpp"

std::vector<Circle> circles;
std::vector<Triangle> triangles;
std::vector<Light> lights; //assumed white for the moment.

glm::vec3 trace(Ray ray);

/**
 * Load mesh data from a Wavefront OBJ file. (exported from blender)
 */
void loadTriangles(const char * meshDataPath) {
  std::vector<glm::vec3> vertices;
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
}

void* beginTracing(void* args) {
  sleep(2);
  loadTriangles("scene.obj");
  lights.push_back(Light(glm::vec3(-100.0f, 0.0f, -25.0f),
                          glm::vec3(2500.0f, 0.0f, 0.0f)));
  lights.push_back(Light(glm::vec3(100.0f, 0.0f, -25.0f),
                          glm::vec3(0.0f, 0.0f, 2500.0f)));

  for(int i = 0; i < PROJ_WIDTH*PROJ_HEIGHT; i++) {
    glm::vec3 color = trace(
                        Ray(glm::vec3(0.0f, 0.0f, -50.0f),
                            glm::normalize(glm::vec3(2.0f*(i%PROJ_WIDTH)-PROJ_WIDTH, 2.0f*(i/PROJ_HEIGHT)-PROJ_HEIGHT, 100.0f)))
                      );
    pixels[i].red = int(fminf(color.x, 1.0f)*255);
    pixels[i].green = int(fminf(color.y, 1.0f)*255);
    pixels[i].blue = int(fminf(color.z, 1.0f)*255);
    glutPostRedisplay();
  }
  // glm::vec3 color = trace(
  //                     Ray(glm::vec3(0.0f, 0.0f, -100.0f),
  //                         glm::normalize(glm::vec3(-50.0f, 50.0f, 100.0f)))
  //                   );
  // printf("Pixel value: (%f,%f,%f)\n", color.x, color.y, color.z );
  return (void*)-1;
}

int main(int argc, char** argv) {
  pthread_t tracingThread;
  printf("Creating thread\n");
  int status = pthread_create(&tracingThread, NULL, beginTracing, (void *)NULL);
  if(status) {
    printf("Pthread start failed\n");
  }

  beginOpenGL(argc, argv);

  pthread_join(tracingThread, NULL);

  printf("Done");
  return 1;
}

/**
 * Given a ray, find the first place it intersects the scene geometry.
 * @param  ray [description]
 * @return
 */
Intersection getClosestIntersection(Ray ray) {
  Intersection closestInters = Intersection();
  float closestDistanceSquared = 0.0f;
  for(std::vector<Triangle>::iterator it = triangles.begin(); it != triangles.end(); ++it) {
    Intersection inters = it->intersect(ray);
    if( inters.hit() ) {
      // printf("Ray hit triangle\n");
      glm::vec3 lineToInters = inters.point - inters.incident.origin;
      float distanceSquared = glm::dot(lineToInters, lineToInters);
      if( distanceSquared < closestDistanceSquared || !closestInters.hit()) {
        closestInters = inters;
        closestDistanceSquared = distanceSquared;
      }
    }
  }
  return closestInters;
}

/**
 * Trace a ray and return the color of the given ray.
 * @param  ray [description]
 * @return
 */
glm::vec3 trace(Ray ray) {
  glm::vec3 color = glm::vec3(0.0f);
  Intersection inters = getClosestIntersection(ray);
  if(inters.hit()) { //if we hit something trace to all the lights to see what color it should be.
    for(std::vector<Light>::iterator lightIter = lights.begin(); lightIter != lights.end(); ++lightIter) {
      glm::vec3 lineToLight = lightIter->location - inters.point;
      Ray shadowRay = Ray(inters.point, glm::normalize(lineToLight));
      Intersection shadowIntersection = getClosestIntersection(shadowRay);
      glm::vec3 lineToShadowRayInters = shadowIntersection.incident.origin - shadowIntersection.point;
      if(!shadowIntersection.hit() // Nothing on path of shadow ray.
        ||  glm::dot(lineToLight, lineToLight) < glm::dot(lineToShadowRayInters, lineToShadowRayInters)) { // We hit something behind the light
          color += lightIter->color/glm::dot(lineToLight, lineToLight); //attenuate light color by inverse square.
      }
    }
  }
  return color;
}
