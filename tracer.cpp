#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "display/simple.hpp"

void* beginTracing(void* args) {
  sleep(2);
  printf("Printing first Pixel value: (%d,%d,%d)\n", pixels[0].red, pixels[0].green, pixels[0].blue );
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
