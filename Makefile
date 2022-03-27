build: 
	g++ src/*.cc -o OpenGL -lGLEW -lGL -lglfw

launch: build
	./OpenGL