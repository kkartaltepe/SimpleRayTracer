#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <vector>

#include "display/simple.hpp"
#include "scene/loader.cpp"

SceneGraph scene;

#define MAX_DEPTH 12

glm::vec3 trace(Ray ray, float distanceTraveled, int maxDepth);

void initSceneData() {
  scene = loadScene("data/room.obj");
  scene.setCamera(Camera(glm::vec3(0.0f, 2.0f, -4.0f),
                         glm::vec3(0.0f, 1.5f, -1.5f),
                         glm::vec3(0.0f, 1.0f, 0.0f),
                         60.0f, PROJ_WIDTH, PROJ_HEIGHT));
  scene.addLight(Light(glm::vec3(0.0f, 2.99f, -0.75f),
                       glm::vec3(1.0f, 1.0f, 1.0f), 20.0f));
}

void* beginTracing(void* args) {
  sleep(2);
  std::vector<Ray> rays = scene.sceneCamera.raysToCast();

  int index = 0;
  for(std::vector<Ray>::iterator rayIter = rays.begin(); rayIter != rays.end(); rayIter++) {
    glm::vec3 color = trace(*rayIter, 0.0f, MAX_DEPTH);
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
Intersection getClosestIntersection(Ray ray, bool isShadowRay) {
  Intersection closestInters = Intersection();
  for(std::vector<Object>::iterator obj = scene.objects.begin(); obj != scene.objects.end(); ++obj) {
    for(std::vector<Triangle>::iterator tri = obj->triangles.begin(); tri != obj->triangles.end(); ++tri) {
      Intersection inters = tri->intersect(ray);
      if( inters.didHit() ) {
        if( inters.distanceTraveled < closestInters.distanceTraveled || !closestInters.didHit() ) {

          closestInters = inters;
          closestInters.object = &*obj;
        }
      }
    }
    for(std::vector<Circle>::iterator circ = obj->circles.begin(); circ != obj->circles.end(); ++circ) {
      Intersection inters = circ->intersect(ray);
      if( inters.didHit() ) {
        if( inters.distanceTraveled < closestInters.distanceTraveled || !closestInters.didHit() ) {

          closestInters = inters;
          closestInters.object = &*obj;
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
glm::vec3 trace(Ray ray, float distanceTraveled, int maxDepth) {
  // if(maxDepth == 0) {
  //   printf("Reached 4 recursions\n");
  // }
  glm::vec3 color = glm::vec3(0.0f);
  Intersection inters = getClosestIntersection(ray, false);
  if(inters.didHit()) { //if we hit something figure out the color.

    color += inters.object->material.aColor; // Ambient lighting.

    if(inters.object->material.opacity < 1.0f  && maxDepth > 0) { // Transmitted light
      float rRatio;
      float cosTheta = glm::dot(inters.incident.direction, inters.normal);
      if(inters.inside) {
        rRatio = inters.object->material.refractiveIndex;
        cosTheta *= -1.0f;
      }
      else {
        rRatio = 1.0f/inters.object->material.refractiveIndex;
      }

      float antiCos = sqrtf(1.0f - (1.0f-cosTheta*cosTheta)*rRatio*rRatio);
      // printf("refraction with ratio: %f, cos: %f, antiCos: %f\n", rRatio, cosTheta, antiCos);
      glm::vec3 refractedDir = rRatio*inters.incident.direction + (cosTheta*rRatio + antiCos)*inters.normal;
      color += trace(Ray(inters.point, refractedDir), inters.distanceTraveled+distanceTraveled, maxDepth-1)*(1.0f-inters.object->material.opacity);
    }

    if(inters.object->material.reflectivity > 0.0f && maxDepth > 0) { // Reflected light
      glm::vec3 projOntoNorm = -glm::dot(inters.incident.direction, inters.normal)*inters.normal;
      glm::vec3 reflectDir = inters.incident.direction + projOntoNorm*2.0f;
      color += trace(Ray(inters.point, reflectDir), inters.distanceTraveled+distanceTraveled, maxDepth-1)*inters.object->material.rColor*inters.object->material.reflectivity;
    }

    if(!inters.inside) { // GLORIOUS IMPROVEMENTS, Diffuse light
      for(std::vector<Light>::iterator lightIter = scene.lights.begin(); lightIter != scene.lights.end(); ++lightIter) {
        glm::vec3 lightDir = lightIter->location - inters.point;
        float distanceToLight = glm::length(lightDir);
        lightDir = glm::normalize(lightDir);

        Ray shadowRay = Ray(inters.point, lightDir);
        Intersection shadowIntersection = getClosestIntersection(shadowRay, true);

        if((!shadowIntersection.didHit() || distanceToLight < shadowIntersection.distanceTraveled )) {// We hit something behind the light
          float totalDistanceTraveled = distanceToLight + inters.distanceTraveled + distanceTraveled; // Distance from light to intersection + inters to eye + recursion(reflected/refracted)

          float difIntensity = glm::dot(lightDir, inters.normal)*lightIter->power;

          glm::vec3 halfAngle = glm::normalize(lightDir - inters.incident.direction); // incident is in the direction from eye, so negate
          float NdotH = std::max(0.0f, glm::dot(inters.normal, halfAngle));
          float specIntensity = powf(NdotH, inters.object->material.specHardness)*lightIter->power; // Spectral hardness of the material

          color += (lightIter->color * inters.object->material.sColor)*specIntensity/powf(totalDistanceTraveled, 2);
          color += (lightIter->color * inters.object->material.dColor)*difIntensity/powf(totalDistanceTraveled, 2);
        }
      }
    }

  }
  return color;
}
