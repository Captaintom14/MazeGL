#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "shaders.h"
#include "maze.h"
#include "Wall.h"

#include <iostream>

// global variables
GLFWwindow* window;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));  // Initialize camera with position
shaders* shaderProgram = nullptr;
maze* Maze = nullptr;

// program functions
void configureGLFW();
int initGLFW();
int initGLEW();
int configureWindow();
void processInput(GLFWwindow *window);
void renderScene();
void setupMaze();

// Configure GLFW
void configureGLFW() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required for macOS
}

// initialize GLFW
int initGLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    return 0;
}

// initialize GLEW
int initGLEW() {
    // Make sure we have a valid OpenGL context
    if (!window) {
        std::cout << "No valid OpenGL context" << std::endl;
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    
    // Check OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    return 0;
}

// configure window
int configureWindow() {
    window = glfwCreateWindow(800, 600, "Maze", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
      float currentFrame = static_cast<float>(glfwGetTime());
      float lastFrame = 0.0f;
      float deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;   

      float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    glm::vec3 oldPosition = camera.Position;


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::FORWARD, 0.01f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::BACKWARD, 0.01f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::LEFT, 0.01f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::RIGHT, 0.01f);
    
    // Check for collision with the maze due to movement of the camera
    if (Maze && Maze->checkCollision(camera.Position)) {
        camera.Position = oldPosition;
    }
}

int main() {
    // Initialize GLFW
    if (initGLFW() != 0) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    configureGLFW();
    
    // Configure window
    if (configureWindow() != 0) {
        std::cout << "Failed to configure window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Initialize GLEW (after window creation)
    if (initGLEW() != 0) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

     

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        processInput(window);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}






