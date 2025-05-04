#ifndef MAZE_H
#define MAZE_H

#pragma once

#include <vector>
#include <stack>
#include <queue>
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <iostream>

#include "Wall.h"
#include "Floor.h"  // Added Floor header
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
    
    // New method to generate a path from start to end
    void generatePath();

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

// Floor objects for the maze floor
vector<Floor*> floorObjects;  // Added floor objects

// Path from start to end point
vector<pair<int, int>> pathCells;
vector<Floor*> pathObjects;  // Objects to render the path

// random number generator
mt19937 rng;

//Method to generate the maze
void initliazeMaze();
void generateMaze();
void createWalls(const string &texturePath);
void createFloors(const string &floorTexturePath);  // Added method for floor creation
void createPathMarkers(); // Create visual markers for the path
void createDirectPath(int startX, int startY, int endX, int endY); // New helper function
};

#endif