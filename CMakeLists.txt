cmake_minimum_required(VERSION 2.8)

project(RayTracer)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)
find_package(GLM)
find_package(Threads)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

ADD_EXECUTABLE(tracer tracer.cpp display/simple.cpp display/simple.hpp)

TARGET_LINK_LIBRARIES(tracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
