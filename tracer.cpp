#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <vector>


#include "display/simple.hpp"
#include "geometry/loader.cpp"
#include "geometry/circle.cpp"
#include "geometry/triangle.cpp"
#include "geometry/ray.hpp"
#include "scene/light.hpp"
#include "scene/camera.cpp"

std::vector<Circle> circles;
std::vector<Triangle> triangles;
std::vector<Light> lights;

Camera camera = Camera(
  glm::vec3(50.0f, 50.0f, -300.0f),
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  120.0f, PROJ_WIDTH, PROJ_HEIGHT);

glm::vec3 trace(Ray ray);

void initSceneData() {
  triangles = loadTriangles("cube.obj");
  lights.push_back(Light(glm::vec3(0.0f, 0.0f, -200.0f),
                          glm::vec3(70000.0f, 0.0f, 0.0f)));
  lights.push_back(Light(glm::vec3(0.0f, 200.0f, 0.0f),
                          glm::vec3(0.0f, 0.0f, 70000.0f)));
  lights.push_back(Light(glm::vec3(0.0f, 200.0f, 0.0f),
                          glm::vec3(0.0f, 0.0f, 70000.0f)));
}

void* beginTracing(void* args) {
  sleep(2);
  std::vector<Ray> rays = camera.raysToCast();
  int index = 0;
  for(std::vector<Ray>::iterator rayIter = rays.begin(); rayIter != rays.end(); rayIter++) {
    glm::vec3 color = trace(*rayIter);
    pixels[index].red = int(fminf(color.x, 1.0f)*255);
    pixels[index].green = int(fminf(color.y, 1.0f)*255);
    pixels[index].blue = int(fminf(color.z, 1.0f)*255);
    index++;
    glutPostRedisplay();
  }
  return (void*)-1;
}

int main(int argc, char** argv) {
  initSceneData();
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
