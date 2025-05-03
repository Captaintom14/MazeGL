#ifndef MAZE_H
#define MAZE_H

#pragma once

#include <vector>
#include <stack>
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <iostream>

#include "Wall.h"
#include "shaders.h"

using namespace std;


class maze
{
public:
    maze(int width, int height, float cellSize, const glm::vec3& position, const string &texturePath ="../assets/brick_wall.png");
    ~maze();

    // starting position of the maze
    glm::vec3 getPosition() const;

    // Ending position of the maze
    glm::vec3 getEndPosition() const;

    // check if the position collides with the maze
    bool checkCollision(const glm::vec3& position) const;

    // render the maze
    void render(shaders* shader, const glm::mat4& view, const glm::mat4& projection);

private:

int width;
int height;
float cellSize;
glm::vec3 position; 

// To track the visited cells during maze generation
vector <vector<bool>> visited;

//2d grid to track walls
vector<vector<vector<bool>>> walls;

// Wall objects to render the maze
vector<Wall*> wallObjects;

// random number generator
mt19937 rng;

//Method to generate the maze
void initliazeMaze();
void generateMaze();
void createWalls(const string &texturePath);
};

#endif