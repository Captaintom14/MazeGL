#include "Floor.h"
#include <iostream>

Floor::Floor(const glm::vec3& position, const glm::vec2& size, const string& texturePath)
    : position(position), size(size)
{
    setupFloor();
    loadTexture(texturePath);
}

Floor::~Floor()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
}

void Floor::setupFloor() {
    // Set up the vertex data for a horizontal floor plane on the XZ plane
    // Generate texture coordinates to ensure proper tiling based on size
    float vertices[] = {
        // Positions           // Texture Coords (tiled 4x4 for better look)
        -0.5f, 0.0f, -0.5f,    0.0f, 0.0f,  // Bottom-left
         0.5f, 0.0f, -0.5f,    4.0f, 0.0f,  // Bottom-right
         0.5f, 0.0f,  0.5f,    4.0f, 4.0f,  // Top-right
        
        -0.5f, 0.0f, -0.5f,    0.0f, 0.0f,  // Bottom-left
         0.5f, 0.0f,  0.5f,    4.0f, 4.0f,  // Top-right
        -0.5f, 0.0f,  0.5f,    0.0f, 4.0f   // Top-left
    };

    // Generate and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the Vertex Array Object
    glBindVertexArray(VAO);

    // Bind the Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Floor::loadTexture(const string& path)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        
        // Upload texture data to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
            
        cout << "Floor texture loaded successfully: " << path << endl;
    }
    else {
        cout << "Failed to load floor texture: " << path << endl;
    }

    stbi_image_free(data);
}

void Floor::render(shaders* shader, const glm::mat4& view, const glm::mat4& projection)
{
    if (!shader) {
        cerr << "Shader not initialized!" << endl;
        return;
    }

    shader->use();

    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size.x, 1.0f, size.y)); // Scale on x and z axes
  
    // Set the model, view, and projection matrices in the shader
    GLuint modelLoc = glGetUniformLocation(shader->ID, "model");
    GLuint viewLoc = glGetUniformLocation(shader->ID, "view");
    GLuint projectionLoc = glGetUniformLocation(shader->ID, "projection");
    
    // Set the uniform values
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Make sure depth test is properly enabled
    glEnable(GL_DEPTH_TEST);
    
    // Save current depth settings
    GLint oldDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
    
    // Use polygon offset to prevent z-fighting with floor
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f); // Negative offset pushes the floor further from camera
    
    // Disable face culling for the floor to ensure it's visible from all angles
    glDisable(GL_CULL_FACE);
    
    // Bind the texture 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLuint textureLoc = glGetUniformLocation(shader->ID, "texture1");
    glUniform1i(textureLoc, 0);

    // Draw the floor
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // Restore previous settings
    glDepthFunc(oldDepthFunc);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
}

void Floor::setPosition(const glm::vec3& position)
{
    this->position = position;
}

glm::vec3 Floor::getPosition() const
{
    return position;
}