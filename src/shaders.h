#ifndef SHADERS_H
#define SHADERS_H

#pragma once

class shaders
{
public:
  
    void createShader(const char* vertexPath, const char* fragmentPath);
    void use();
    unsigned int ID;
    
private:

};

#endif