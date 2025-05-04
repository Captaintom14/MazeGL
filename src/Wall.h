#ifndef WALL_H
#define WALL_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>  // Added for glm::value_ptr
#include "stb_image.h"  // Local include now available in src directory
#include <string>

#include "shaders.h"  // Fixed to use quotes for local include
using namespace std;

#pragma once

class Wall
{
public:
     // Constructor
    Wall(const glm::vec3& position, const glm::vec3& size, const string& texturePath);
    
    // Destructor

    ~Wall();
    
     // Set the position of the wall
     void setPosition(const glm::vec3& position);
     glm::vec3 getPosition() const;
 
     // Set the size of the wall
    void setSize(const glm::vec3& size);
    glm::vec3 getSize() const;

    // set the rotation of the wall
    void setRotation(float angle, const glm::vec3& axis);
    glm::vec3 getRotation() const;

    // render the wall
    void render(shaders* shader, const glm::mat4& view, const glm::mat4& projection);

private:
unsigned int VAO, VBO;
unsigned int textureID;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;

    float rotationAngle;

    void setupWall();
    void loadTexture(const string& path);
    glm::mat4 modelMatrix;
};

#endif