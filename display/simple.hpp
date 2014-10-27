#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#ifndef SIMPLE_H
#define SIMPLE_H
struct Pixel {
  unsigned char red,green,blue;
};

int beginOpenGL(int argc, char* argv[] );

void initOpenGLData(void);
void renderScene(void);
void changeSize(int width, int height);

#endif /* end of include guard: SIMPLE_H */
