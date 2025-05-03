#include "Wall.h"
#include <iostream>



Wall::Wall(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color, const string& texturePath)
    : position(position), size(size), color(color)
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
    // Set up the vertex data
    float vertices[] ={
        // positions          // texture coords
        -0.5f, -0.5f,  0.0f,      0.0f, 0.0f,
        0.5f, -0.5f,  0.0f,      1.0f, 0.0f,
        0.5f,  0.5f,  0.0f,      1.0f, 1.0f,
       
       -0.5f, -0.5f,  0.0f,      0.0f, 0.0f,
        0.5f,  0.5f,  0.0f,      1.0f, 1.0f,
       -0.5f,  0.5f,  0.0f,      0.0f, 1.0f
    };

    //set up the indices
    unsigned int indices[] = {
        0, 1, 2,
        3, 4, 5
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
            // set texture wrapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            cout << "Texture loaded successfully: " << path << endl;
    }
    else {
        cout << "Failed to load texture: " << path << endl;
    }

    stbi_image_free(data);
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




