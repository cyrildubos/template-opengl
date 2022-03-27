#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "OpenGL"

const char *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_Position;"
    "layout (location = 1) in vec3 a_Color;"
    "out vec3 v_Color;"
    "void  main() { "
    "  gl_Position = vec4(a_Position, 1.0); "
    "  v_Color = a_Color; "
    "}";

const char *fragment_shader_source =
    "#version 330 core\n"
    "in vec3 v_Color;"
    "out vec4 f_Color;"
    "void main() {"
    "  f_Color = vec4(v_Color, 1.0);"
    "}";

void CheckShader(GLuint shader) {
  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

  if (!compile_status) {
    std::cerr << "Error: failed to compile shader" << std::endl;

    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar info_log[info_log_length];
    glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

    std::cerr << info_log << std::endl;
  }
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Error: failed to initialize GLFW" << std::endl;

    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (!window) {
    std::cerr << "Error: failed to create window" << std::endl;

    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Error: failed to initialize GLEW" << std::endl;

    glfwTerminate();

    return -1;
  }

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  CheckShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  CheckShader(fragment_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);
  glUseProgram(program);

  GLfloat data[] = {
      // positions        // colors
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  //
      0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  //
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  //
  };

  GLuint indices[] = {
      0, 1, 2,  //
  };

  GLuint vertex_array;
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);

  GLuint array_buffer;
  glGenBuffers(1, &array_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

  GLint position_attribute = glGetAttribLocation(program, "a_Position");
  glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(GLfloat) * 6, (void *)0);
  glEnableVertexAttribArray(position_attribute);

  GLint color_attribute = glGetAttribLocation(program, "a_Color");
  glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(GLfloat) * 6, (void *)(sizeof(GLfloat) * 3));
  glEnableVertexAttribArray(color_attribute);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint),
                   GL_UNSIGNED_INT, indices);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}