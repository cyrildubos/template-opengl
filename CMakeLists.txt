cmake_minimum_required(VERSION 3.0.0)
project(template-opengl VERSION 0.1.0)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(main src/main.c)
target_link_libraries(main OpenGL GLEW glfw)
