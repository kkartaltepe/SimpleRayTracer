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
  glm::dvec3(0.0f, 150.0f, -200.0f),
  glm::dvec3(0.0f, 0.0f, 0.0f),
  glm::dvec3(0.0f, 1.0f, 0.0f),
  90.0f, PROJ_WIDTH, PROJ_HEIGHT);

glm::dvec3 trace(Ray ray);

void initSceneData() {
  // triangles.push_back(Triangle(glm::vec3(50.0f, -100.0f, 100.0f),
  //                              glm::vec3(100.0f, 100.0f, 100.0f),
  //                             //  glm::vec3(-100.0f, 100.0f, 100.0f)));
  // triangles.push_back(Triangle(glm::vec3(0.0f, 100.0f, 100.0f),
  //                              glm::vec3(-100.0f, -100.0f, 100.0f),
  //                              glm::vec3(100.0f, -100.0f, 100.0f)));
  triangles = loadTriangles("cube.obj");
  lights.push_back(Light(glm::dvec3(100.0, 0.0, -150.0),
                          glm::dvec3(0.0, 0.0, 8000.0)));
  lights.push_back(Light(glm::dvec3(-100.0, 0.0, -150.0),
                          glm::dvec3(0.0, 8000.0, 0.0)));
  lights.push_back(Light(glm::dvec3(0.0, 200.0, 0.0),
                          glm::dvec3(8000.0, 0.0, 0.0)));
}

void* beginTracing(void* args) {
  sleep(2);
  std::vector<Ray> rays = camera.raysToCast();
  // std::vector<Ray> rays;
  // for(int y = PROJ_HEIGHT; y > 0; --y) {
  //   for(int x = 0; x < PROJ_WIDTH; ++x) {
  //     Ray toCast = Ray(glm::vec3(0.0f, 0.0f, -150.0f),
  //                      glm::vec3((float)x-PROJ_WIDTH/2.0f, (float)y-PROJ_WIDTH/2.0f, 150.0f));
  //     // printf("from (%f,%f,%f) to (%f, %f, 0)\n", toCast.origin.x, toCast.origin.y, toCast.origin.z, toCast.direction.x, toCast.direction.y);
  //     rays.push_back(toCast);
  //   }
  // }

  int index = 0;
  for(std::vector<Ray>::iterator rayIter = rays.begin(); rayIter != rays.end(); rayIter++) {
    glm::dvec3 color = trace(*rayIter);
    pixels[index].red = int(fmin(color.x, 1.0)*255);
    pixels[index].green = int(fmin(color.y, 1.0)*255);
    pixels[index].blue = int(fmin(color.z, 1.0)*255);
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
    if( inters.didHit() ) {
      glm::dvec3 lineToInters = inters.point - inters.incident.origin;
      float distanceSquared = glm::dot(lineToInters, lineToInters);
      if( distanceSquared < closestDistanceSquared || !closestInters.didHit()) {
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
glm::dvec3 trace(Ray ray) {
  glm::dvec3 color = glm::dvec3(0.0f);
  Intersection inters = getClosestIntersection(ray);
  if(inters.didHit()) { //if we hit something trace to all the lights to see what color it should be.
    for(std::vector<Light>::iterator lightIter = lights.begin(); lightIter != lights.end(); ++lightIter) {
      glm::dvec3 lineToLight = lightIter->location - inters.point;
      Ray shadowRay = Ray(inters.point, lineToLight);
      //Check if shadow ray is "inside" the triangle (fix culling)
      Intersection shadowIntersection = getClosestIntersection(shadowRay);
      glm::dvec3 lineToShadowRayInters = shadowIntersection.incident.origin - shadowIntersection.point;
      if(!shadowIntersection.didHit() // Nothing on path of shadow ray.
        ||  glm::dot(lineToLight, lineToLight) < glm::dot(lineToShadowRayInters, lineToShadowRayInters)) { // We hit something behind the light
          color += lightIter->color/glm::dot(lineToLight, lineToLight); //attenuate light color by inverse square.
      }
    }
  }
  return color;
}
