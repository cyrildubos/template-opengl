project('template-opengl', 'c')

glew_dep = dependency('glew')
glfw_dep = dependency('glfw3')
cglm_dep = dependency('cglm')

executable('main', 'src/main.c', dependencies: [glew_dep, glfw_dep, cglm_dep], link_args : '-lm')