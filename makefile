CC = g++
CFLAGS = -std=c++17 -I/opt/homebrew/opt/glew/include -I/opt/homebrew/opt/glfw/include -I/opt/homebrew/include -I/opt/homebrew/opt/freeglut/include -I/opt/homebrew/opt/freetype/include -I/opt/homebrew/opt/assimp/include -I/opt/homebrew/opt/glm/include
LDFLAGS = -L/opt/homebrew/opt/glew/lib -L/opt/homebrew/opt/glfw/lib -L/opt/homebrew/opt/freeglut/lib -L/opt/homebrew/opt/freetype/lib -L/opt/homebrew/opt/assimp/lib -lGLEW -lglfw -framework OpenGL -lassimp -DGL_SILENCE_DEPRECATION

SRC = src/main.cpp src/shaders.cpp src/Camera.cpp src/Wall.cpp src Maze.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = Maze

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)