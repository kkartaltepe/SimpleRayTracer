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
  scene = loadScene("data/room.obj");
  scene.setCamera(Camera(glm::vec3(0.0f, 2.0f, -4.0f),
                         glm::vec3(0.0f, 1.5f, -1.5f),
                         glm::vec3(0.0f, 1.0f, 0.0f),
                         60.0f, 512, 512));
  scene.addLight(Light(glm::vec3(0.0f, 2.0f, -1.5f),
                       glm::vec3(1.0f, 1.0f, 1.0f), 20.0f));
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
  for(std::vector<Object>::iterator obj = scene.objects.begin(); obj != scene.objects.end(); ++obj) {
    for(std::vector<Triangle>::iterator tri = obj->triangles.begin(); tri != obj->triangles.end(); ++tri) {
      Intersection inters = tri->intersect(ray);
      if( inters.didHit() ) {
        glm::vec3 lineToInters = inters.point - inters.incident.origin;
        float distanceSquared = glm::dot(lineToInters, lineToInters);
        if( distanceSquared < closestDistanceSquared || !closestInters.didHit()) {
          closestInters = inters;
          closestInters.object = &*obj;
          closestDistanceSquared = distanceSquared;
        }
      }
    }
    for(std::vector<Circle>::iterator circ = obj->circles.begin(); circ != obj->circles.end(); ++circ) {
      Intersection inters = circ->intersect(ray);
      if( inters.didHit() ) {
        glm::vec3 lineToInters = inters.point - inters.incident.origin;
        float distanceSquared = glm::dot(lineToInters, lineToInters);
        if( distanceSquared < closestDistanceSquared || !closestInters.didHit()) {
          closestInters = inters;
          closestInters.object = &*obj;
          closestDistanceSquared = distanceSquared;
        }
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
      if((!shadowIntersection.didHit() || lineToLightLengthSquared < shadowRayIntersLengthSquared)) {// We hit something behind the light
          float distanceTraveled = sqrtf(lineToLightLengthSquared) + glm::length(lineToEye); //used in attenuation

          float difIntensity = glm::dot(glm::normalize(lineToLight), inters.normal)*lightIter->power;

          glm::vec3 halfAngle = glm::normalize(glm::normalize(lineToLight) - inters.incident.direction); //incident is in the direction from eye, so negate
          float NdotH = std::max(0.0f, glm::dot(inters.normal, halfAngle));
          float specIntensity = powf(NdotH, inters.object->material.specHardness)*lightIter->power; //Spectral hardness of the material

          color += inters.object->material.aColor;
          color += (lightIter->color * inters.object->material.sColor)*specIntensity/powf(distanceTraveled, 2);
          color += (lightIter->color * inters.object->material.dColor)*difIntensity/powf(distanceTraveled, 2);
      }
    }
  }
  return color;
}
