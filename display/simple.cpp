#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctime>
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "shaderutil.cpp"

void initThings(void);
void renderScene(void);
void changeSize(int width, int height);

const int PROJ_WIDTH = 512, PROJ_HEIGHT = 512;
const float PROJ_RATIO = 1.0*PROJ_WIDTH/PROJ_HEIGHT;

struct Pixel {
  unsigned char red,green,blue;
};
Pixel pixels[PROJ_WIDTH*PROJ_HEIGHT];

GLuint programID,vertexArrayID, uvArrayID, textureID, textureSampler, mvpMatrix;

glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f*PROJ_RATIO, 1.0f*PROJ_RATIO, 1.0f, -1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);

//Define a sexy to view
static const GLfloat vertexArrayData[] = {
  -1.0f, -1.0f, 0.0f,
  -1.0f, 1.0f, 0.0f,
   1.0f, 1.0f, 0.0f,
   1.0f, -1.0f, 0.0f
};
static const GLfloat uvArrayData[] = {
  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,
  1.0f, 0.0f
};

//To be used with pthreads
void *beginOpenGL( const Pixel* pixels, int argc, char* argv[] ) {
  glutInit(&argc, argv);
  glutInitWindowPosition(-1, -1); //Default, Default
  glutInitWindowSize(PROJ_WIDTH, PROJ_HEIGHT);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutCreateWindow("SimpleGL Test");

  glewExperimental=true; // https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW_.28OpenGL_Extension_Wrangler.29
  if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return (void*)-1;
  }

  initThings();

  //Defines the standard rendering function
  glutDisplayFunc(renderScene);
  //Defines a function to be called when the window is resized
  glutReshapeFunc(changeSize);

  glutMainLoop();

  glDeleteBuffers(1, &vertexArrayID);
  glDeleteBuffers(1, &uvArrayID);
  glDeleteProgram(programID);
  glDeleteTextures(1, &textureID);
  return (void*)1;
}

void initThings(void) {
  //Give some color to things.
  for(int i = 0; i < PROJ_WIDTH*PROJ_HEIGHT; i++) {
    pixels[i].red = rand() % 255;
    pixels[i].green = rand() % 255;
    pixels[i].blue = rand() % 255;
  }

  // Load shaders
  programID = LoadShaders("vertex.glsl", "fragment.glsl");

  glGenBuffers(1, &vertexArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexArrayID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArrayData), vertexArrayData, GL_STATIC_DRAW);

  glGenBuffers(1, &uvArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, uvArrayID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uvArrayData), uvArrayData, GL_STATIC_DRAW);

  // Create one OpenGL texture
  glGenTextures(1, &textureID);
  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);
  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PROJ_WIDTH, PROJ_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  //Setup mipmaping and magnification filter.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  //Grab uniform identifiers before rendering
  textureSampler = glGetUniformLocation(programID, "textureSampler");
  mvpMatrix = glGetUniformLocation(programID, "mvp");
}

void renderScene(void) {
  //Reload the texture.
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PROJ_WIDTH, PROJ_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

  //Load our shaders
  glUseProgram(programID);

  //Send uniform data to shaders
  glUniformMatrix4fv(mvpMatrix, 1, GL_FALSE, &mvp[0][0]);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(textureSampler, 0);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexArrayID);
  glVertexAttribPointer(
    0,        //attribute number in shader
    3,        //size of an attribute
    GL_FLOAT, //type
    GL_FALSE, //normalized?
    0,        //Stride
    (void*)0  //Offset
  );
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uvArrayID);
  glVertexAttribPointer(
    1,        //attribute number in shader
    2,        //size of an attribute
    GL_FLOAT, //type
    GL_FALSE, //normalized?
    0,        //Stride
    (void*)0  //Offset
  );

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

void changeSize(int width, int height){
  float windowRatio = 1.0*width/height;
  if(windowRatio > PROJ_RATIO){
    glViewport((width-height*PROJ_RATIO)/2, 0, height*PROJ_RATIO, height);
  }else {
    glViewport(0, (height-width/PROJ_RATIO)/2, width, width/PROJ_RATIO);
  }

  projection = glm::ortho(-1.0f, 1.0f, -1.0f*PROJ_RATIO, 1.0f*PROJ_RATIO, 1.0f, -1.0f);

  // Camera matrix
  // glm::mat4 view = glm::lookAt(
  //     glm::vec3(4,3,3), // Location
  //     glm::vec3(0,0,0), // LooksAt
  //     glm::vec3(0,1,0)  // Up
  // );

  // model = glm::rotate(model, 3.14159f/200.0f, glm::vec3(0, 0, 1.0f));

  mvp = projection * view * model;
}
