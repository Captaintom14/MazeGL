CC = g++
CFLAGS = -std=c++17 -I/opt/homebrew/opt/glew/include -I/opt/homebrew/opt/glfw/include -I/opt/homebrew/include -I/opt/homebrew/opt/freeglut/include -I/opt/homebrew/opt/freetype/include -I/opt/homebrew/opt/assimp/include -I/opt/homebrew/opt/glm/include
LDFLAGS = -L/opt/homebrew/opt/glew/lib -L/opt/homebrew/opt/glfw/lib -L/opt/homebrew/opt/freeglut/lib -L/opt/homebrew/opt/freetype/lib -L/opt/homebrew/opt/assimp/lib -lGLEW -lglfw -framework OpenGL -lassimp -DGL_SILENCE_DEPRECATION

SRC = src/main.cpp src/shaders.cpp src/Camera.cpp src/Wall.cpp src/maze.cpp src/stb_image_impl.cpp src/Floor.cpp
BUILD_DIR = build
OBJ = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
TARGET = Maze

all: create_build_dir $(TARGET)

create_build_dir:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	@if [ -d "$(BUILD_DIR)" ]; then rmdir $(BUILD_DIR); fi