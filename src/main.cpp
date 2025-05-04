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
shaders* wallShader = nullptr;
maze* Maze = nullptr;
float lastFrame = 0.0f;
float deltaTime = 0.0f;

// Mouse input handling variables
bool firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;

// program functions
void configureGLFW();
int initGLFW();
int initGLEW();
int configureWindow();
void processInput(GLFWwindow *window);
void renderScene();
void setupMaze();
void setupShaders();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;   

    // Store original position to revert to if collision occurs
    glm::vec3 originalPosition = camera.Position;
    
    // Adjust movement speed
    float cameraSpeed = 2.0f * deltaTime;
    
    // Add sprint capability
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 1.5f;
    }
    
    // Toggle free movement mode with F key
    static bool freeMovementMode = false;
    static bool fKeyPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!fKeyPressed) {
            freeMovementMode = !freeMovementMode;
            fKeyPressed = true;
            if (freeMovementMode){
                cameraSpeed *= 2.5f; // Increase speed in free movement mode
                std::cout << "Free movement mode: ON" << std::endl;
            }
                
            else
                std::cout << "Free movement mode: OFF" << std::endl;
        }
    } else {
        fKeyPressed = false;
    }
    
    if (freeMovementMode) {
        // Free movement in all directions and no collisions
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.Position += camera.Front * cameraSpeed;
            
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.Position -= camera.Front * cameraSpeed;
            
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
            
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
            
        // Vertical movement with Q and E keys
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        // Move down
            camera.Position -= glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
            
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        // Move up
            camera.Position += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
    }
    else {
        // Regular maze movement (constrained to XZ plane)
        // Move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // Calculate forward movement in XZ plane only
            glm::vec3 front = camera.Front;
            front.y = 0.0f; 
            front = glm::normalize(front); 
            camera.Position += front * cameraSpeed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm::vec3 front = camera.Front;
            front.y = 0.0f;
            front = glm::normalize(front);
            camera.Position -= front * cameraSpeed;
        }
        // Move left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
        }
        // Move right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
        }
        
        // Check collision and revert position if needed
        if (Maze && Maze->checkCollision(camera.Position)) {
            camera.Position = originalPosition;
        }
        
        // Keep camera at consistent eye height in regular mode
        camera.Position.y = 0.5f;
    }
    
    // Reset position if R is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.Position = Maze->getPosition();
        camera.Position.y = 0.5f; // Reset to eye level
    }
}

void setupMaze(){
    
    wallShader = new shaders();
    wallShader->createShader("shaders/wall.vs", "shaders/wall.fs");
 
    // Initialize the maze with a larger size for more exploration
    int mazeWidth = 15;
    int mazeHeight = 15;
    float cellSize = 1.0f;

    Maze = new maze(mazeWidth, mazeHeight, cellSize, glm::vec3(0.0f, 0.0f, 0.0f), "assets/brick_wall.png");

    // Position the maze in the world for the camera
    camera.Position = Maze->getPosition();
    camera.Position.y = 0.5f; // Eye level height

}


void renderScene(){
    // Set a nice sky blue gradient background
    float timeValue = glfwGetTime();
    float blueIntensity = 0.7f + 0.1f * sin(timeValue * 0.2f); // Subtle blue variation over time
    
    // Create gradient sky effect - sky blue at the top, lighter at the horizon
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw sky gradient using a full-screen quad
    GLuint skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    
    // Full-screen quad vertices (NDC coordinates)
    float skyVertices[] = {
        // positions         // colors (top = blue, bottom = light blue)
        -1.0f,  1.0f, 0.0f,  0.3f, 0.5f, 0.9f,  // top-left
         1.0f,  1.0f, 0.0f,  0.3f, 0.5f, 0.9f,  // top-right
         1.0f, -1.0f, 0.0f,  0.7f, 0.9f, 1.0f,  // bottom-right
         1.0f, -1.0f, 0.0f,  0.7f, 0.9f, 1.0f,  // bottom-right
        -1.0f, -1.0f, 0.0f,  0.7f, 0.9f, 1.0f,  // bottom-left
        -1.0f,  1.0f, 0.0f,  0.3f, 0.5f, 0.9f   // top-left
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Use a simple shader for the sky
    if (!shaderProgram) {
        shaderProgram = new shaders();
        shaderProgram->createShader("shaders/sky.vs", "shaders/sky.fs");
    }
    
    // Draw sky 
    glDisable(GL_DEPTH_TEST);
    shaderProgram->use();
    glBindVertexArray(skyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    
    // Clean up temporary resources
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    
    // Clear just the depth buffer for our 3D scene rendering
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);

    // Render the maze on top of our sky background
    if (Maze){
        Maze->render(wallShader, view, projection);
    }
    else {
        std::cout << "Maze not initialized!" << std::endl;
    }

    if (Maze){
        glm::vec3 endPos = Maze->getEndPosition();
        float distanceToEnd = glm::length(camera.Position - endPos);
    }
}

// mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { // this boolean variable is set to false after the first mouse movement
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y coordinates go from bottom to top
    
    // Adjust sensitivity - may need to be tuned based on your mouse/display
    const float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// framebuffer size callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
        return -1;
    }
    
    // Initialize GLEW (after window creation)
    if (initGLEW() != 0) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Enable depth testing with proper parameters
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable backface culling for better performance and rendering
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Additional rendering settings for better visual quality
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupMaze();
    
    // Set the mouse callback function
    glfwSetCursorPosCallback(window, mouse_callback);
    
    // Set the framebuffer size callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Capture the cursor for first-person camera control
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Generate a path from start to end if the maze supports it
    if (Maze) {
        Maze->generatePath();
    }

    // Display instructions
    std::cout << "\n===== MazeGL Controls =====\n";
    std::cout << "Use WASD keys to navigate through the maze." << std::endl;
    std::cout << "Use mouse to look around." << std::endl;
    std::cout << "Find your way to the exit at the opposite corner!\n" << std::endl;
    
    std::cout << "===== Free Movement Mode =====\n";
    std::cout << "Press F to toggle free movement mode ON/OFF" << std::endl;
    std::cout << "In free movement mode:" << std::endl;
    std::cout << "  - WASD: Move in any direction the camera is facing" << std::endl;
    std::cout << "  - Q/E: Move down/up vertically" << std::endl;
    std::cout << "  - Collisions are disabled for testing purposes\n" << std::endl;
    
    std::cout << "Press R to reset position" << std::endl;
    std::cout << "Press ESC to exit the application" << std::endl;
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        processInput(window);
        // Render the scene
        renderScene();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    delete shaderProgram;
    delete Maze;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}






