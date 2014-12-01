#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <vector>


#include "display/simple.hpp"
#include "scene/loader.cpp"

SceneGraph scene;

glm::vec3 trace(Ray ray);

void initSceneData() {
  scene = loadScene("simple.scene");
}

void* beginTracing(void* args) {
  sleep(2);
  std::vector<Ray> rays = scene.sceneCamera.raysToCast();

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
  for(std::vector<Triangle>::iterator it = scene.triangles.begin(); it != scene.triangles.end(); ++it) {
    Intersection inters = it->intersect(ray);
    if( inters.didHit() ) {
      glm::vec3 lineToInters = inters.point - inters.incident.origin;
      float distanceSquared = glm::dot(lineToInters, lineToInters);
      if( distanceSquared < closestDistanceSquared || !closestInters.didHit()) {
        closestInters = inters;
        closestDistanceSquared = distanceSquared;
      }
    }
  }
  for(std::vector<Circle>::iterator it = scene.circles.begin(); it != scene.circles.end(); ++it) {
    Intersection inters = it->intersect(ray);
    if( inters.didHit() ) {
      glm::vec3 lineToInters = inters.point - inters.incident.origin;
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
glm::vec3 trace(Ray ray) {
  glm::vec3 color = glm::vec3(0.0f);
  Intersection inters = getClosestIntersection(ray);
  if(inters.didHit()) { //if we hit something trace to all the lights to see what color it should be.
    for(std::vector<Light>::iterator lightIter = scene.lights.begin(); lightIter != scene.lights.end(); ++lightIter) {
      glm::vec3 lineToLight = lightIter->location - inters.point;
      glm::vec3 lineToEye = inters.incident.origin - inters.point; //Used in attenuation
      Ray shadowRay = Ray(inters.point, lineToLight);
      Intersection shadowIntersection = getClosestIntersection(shadowRay);
      glm::vec3 lineToShadowRayInters = shadowIntersection.incident.origin - shadowIntersection.point;
      float shadowRayIntersLengthSquared = glm::dot(lineToShadowRayInters, lineToShadowRayInters);
      float lineToLightLengthSquared = glm::dot(lineToLight, lineToLight);
      if((!shadowIntersection.didHit()
          || lineToLightLengthSquared < shadowRayIntersLengthSquared)) {// We hit something behind the light
          float distanceTraveled = sqrtf(lineToLightLengthSquared) + glm::length(lineToEye); //used in attenuation
          color += lightIter->color/powf(distanceTraveled, 2);
      }
    }
  }
  return color;
}
