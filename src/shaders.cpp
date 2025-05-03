#include "shaders.h"
#include <"glm/glm.hpp">

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


void shaders::createShader(const char* vertexPath, const char* fragmentPath){

    cout << "Creating shader..." << endl;

    // 1. read vertex shader code from file
    cout << "Reading vertex shader code from file..." << endl;
    string vertexCode;
    ifstream vShaderFile;
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit); // enable exceptions
    try {
        vShaderFile.open(vertexPath);
        stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vShaderFile.close();
        vertexCode = vShaderStream.str();
    }
    catch (ifstream::failure e) {
        cout << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESSFULLY_READ" << endl;
    }
    const char* vShaderCode = vertexCode.c_str();

    // Done reading vertex shader code
    //-------------------------------------------------
    //-------------------------------------------------
    // 2. read fragment shader code from file
    cout << "Reading fragment shader code from file..." << endl;
     
    string fragmentCode;
    ifstream fShaderFile;
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit); // enable exceptions
    try {
        fShaderFile.open(fragmentPath);
        stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fShaderFile.close();
        fragmentCode = fShaderStream.str();
    }
    catch (ifstream::failure e) {
        cout << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESSFULLY_READ" << endl;
    }
    const char* fShaderCode = fragmentCode.c_str();
    // Done reading fragment shader code
    //-------------------------------------------------
    //-------------------------------------------------

    // compile shaders
    cout << "Compiling shaders..." << endl;
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // 3. vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Check for compilation errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // 4. fragment shader
    fragment = glcreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Check for compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // 5. shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // Check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // delete the shaders since they're part of the program now
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    cout << "Shader created successfully!" << endl;
}


void shaders::use() {
    glUseProgram(ID);
}
