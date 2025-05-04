#ifndef FLOOR_H
#define FLOOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <string>

#include "shaders.h"
using namespace std;

class Floor
{
public:
    // Constructor
    Floor(const glm::vec3& position, const glm::vec2& size, const string& texturePath);
    
    // Destructor
    ~Floor();
    
    // Set the position of the floor
    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;
 
    // render the floor
    void render(shaders* shader, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO, VBO;
    unsigned int textureID;
    glm::vec3 position;
    glm::vec2 size;  // x and z dimensions

    void setupFloor();
    void loadTexture(const string& path);
};

#endif