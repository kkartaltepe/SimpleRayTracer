#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
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

void* beginTracing(void* args) {
  sleep(2);
  triangles.push_back(Triangle(glm::vec3(-100.0, -100.0f, 0.0f),
                               glm::vec3(-100.0f, 100.0f, 0.0f),
                               glm::vec3(100.0f, 100.0f, 0.0f)));
  triangles.push_back(Triangle(glm::vec3(-100.0f, -100.0f, 0.0f),
                              glm::vec3(100.0f, 100.0f, 0.0f),
                              glm::vec3(100.0f, -100.0f, 0.0f)));
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
          color += lightIter->color/glm::dot(lineToLight, lineToLight); //Assume all lights are white attenuate by inverse square.
      }
    }
  }
  return color;
}
