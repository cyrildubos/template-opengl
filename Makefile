build: 
	gcc src/*.c -o OpenGL -lGLEW -lGL -lglfw

launch: build
	./OpenGL