cmake_minimum_required(VERSION 3.0.0)
project(sdl2opengl VERSION 0.1.0)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_LIBRARIES})
