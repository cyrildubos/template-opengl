#include <stdio.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "OpenGL"

const GLchar *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_Position;"
    "layout (location = 1) in vec3 a_Color;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"
    "out vec3 v_Color;"
    "void  main() { "
    "  gl_Position = projection * model * view * vec4(a_Position, 1.0); "
    "  v_Color = a_Color; "
    "}";

const GLchar *fragment_shader_source = "#version 330 core\n"
                                       "in vec3 v_Color;"
                                       "out vec4 f_Color;"
                                       "void main() {"
                                       "  f_Color = vec4(v_Color, 1.0);"
                                       "}";

void check_shader_compilation(GLuint shader) {
  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

  if (!compile_status) {
    fprintf(stderr, "Error: failed to compile shader\n");

    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar info_log[info_log_length];
    glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

    fprintf(stderr, info_log);
  }
}

vec3 camera_position = {0.0f, 0.0f, 1.0f};
vec3 camera_direction = {0.0f, 0.0f, -1.0f};
vec3 camera_up = {0.0f, 1.0f, 0.0f};

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  const float camera_speed = 0.05f;

  vec3 camera_d;
  vec3 camera_u;
  vec3 camera_r;

  glm_vec3_scale(camera_direction, camera_speed, camera_d);
  glm_vec3_scale(camera_up, camera_speed, camera_u);
  glm_vec3_cross(camera_up, camera_direction, camera_r);
  glm_vec3_scale(camera_r, camera_speed, camera_r);

  if (key == GLFW_KEY_W && action == GLFW_REPEAT)
    glm_vec3_add(camera_position, camera_d, camera_position);
  if (key == GLFW_KEY_S && action == GLFW_REPEAT)
    glm_vec3_sub(camera_position, camera_d, camera_position);
  if (key == GLFW_KEY_A && action == GLFW_REPEAT)
    glm_vec3_add(camera_position, camera_r, camera_position);
  if (key == GLFW_KEY_D && action == GLFW_REPEAT)
    glm_vec3_sub(camera_position, camera_r, camera_position);
  if (key == GLFW_KEY_Q && action == GLFW_REPEAT)
    glm_vec3_rotate(camera_direction, +camera_speed, camera_up);
  if (key == GLFW_KEY_E && action == GLFW_REPEAT)
    glm_vec3_rotate(camera_direction, -camera_speed, camera_up);
}

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Error: failed to initialize GLFW\n");

    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (!window) {
    fprintf(stderr, "Error: failed to create window\n");

    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Error: failed to initialize GLEW\n");

    glfwTerminate();

    return -1;
  }

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fragment_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glUseProgram(program);

  GLfloat data[] = {
      // positions        // colors
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, //
      0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, //
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //
  };

  GLuint indices[] = {
      0, 1, 2, //
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

  mat4 model_matrix;
  mat4 view_matrix;
  mat4 projection_matrix;

  GLuint model_location = glGetUniformLocation(program, "model");
  GLuint view_location = glGetUniformLocation(program, "view");
  GLuint projection_location = glGetUniformLocation(program, "projection");

  while (!glfwWindowShouldClose(window)) {
    glm_mat4_identity(model_matrix);
    glm_look(camera_position, camera_direction, camera_up, view_matrix);
    glm_perspective(45.0f, 16.0f / 9.0f, 0.001f, 100.0f, projection_matrix);

    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(view_location, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE,
                       &projection_matrix[0][0]);

    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint),
                   GL_UNSIGNED_INT, indices);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}