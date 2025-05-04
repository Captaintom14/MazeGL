#include "Wall.h"
#include <iostream>



Wall::Wall(const glm::vec3& position, const glm::vec3& size,  const string& texturePath)
    : position(position), size(size)
{
    setupWall();
    loadTexture(texturePath);
}


Wall::~Wall()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
}


void Wall :: setupWall(){
    // Set up the vertex data for a full 3D cube
    float vertices[] = {
        // positions          // texture coords
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left
        
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
        
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // top-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
        
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // top-left
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f   // bottom-left
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

void Wall::loadTexture(const string& path)
{
   glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data){
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        
        // This is the missing part - actually upload texture data to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
            
        cout << "Texture loaded successfully: " << path << endl;
    }
    else {
        cout << "Failed to load texture: " << path << endl;
    }

    stbi_image_free(data);
}

void Wall::render(shaders* shader, const glm::mat4& view, const glm::mat4& projection)
{
   if (!shader){
    cerr << "Shader not initialized!" << endl;
    return;
   }

   shader->use();

   // create model matrix
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, position);
   model = glm::scale(model, size);
   model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
  
   // Set the model, view, and projection matrices in the shader
   GLuint modelLoc = glGetUniformLocation(shader->ID, "model");
   GLuint viewLoc = glGetUniformLocation(shader->ID, "view");
   GLuint projectionLoc = glGetUniformLocation(shader->ID, "projection");
   
   // Set the uniform values
   glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
   glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
   glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
   // Ensure depth test is enabled with proper parameters
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   
   // Disable backface culling to make walls visible from all angles
   glDisable(GL_CULL_FACE);
   
   // Prevent z-fighting by using polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1.0f, 1.0f);
   
   // Bind the texture 
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textureID);
   GLuint textureLoc = glGetUniformLocation(shader->ID, "texture1");
   glUniform1i(textureLoc, 0);

   // Draw the wall - drawing all 36 vertices (6 faces with 6 vertices each)
   glBindVertexArray(VAO);
   glDrawArrays(GL_TRIANGLES, 0, 36);
   glBindVertexArray(0);
   
   // Disable polygon offset after drawing
   glDisable(GL_POLYGON_OFFSET_FILL);
}



void Wall::setPosition(const glm::vec3& positionSet)
{
    position = positionSet;
}
glm::vec3 Wall::getPosition() const
{
    return position;
}
void Wall::setSize(const glm::vec3& size)
{
    this->size = size;
}
glm::vec3 Wall::getSize() const
{
    return size;
}
void Wall::setRotation(float angle, const glm::vec3& axis)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}




