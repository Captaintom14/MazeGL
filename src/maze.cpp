#include "maze.h"

maze::maze(int width, int height, float cellSize, const glm::vec3& position, const string &texturePath)
    : width(width), height(height), cellSize(cellSize), position(position)
{
    // Initialize the random number generator
    rng.seed(static_cast<unsigned int>(time(nullptr)));

    // Initialize the maze
    initliazeMaze();
    generateMaze();
    createWalls(texturePath);
}

//destructor
maze::~maze()
{
   for (auto wall : wallObjects){
         delete wall;
   }
   wallObjects.clear();
}

// initialize the maze (the grid)
void maze::initliazeMaze()
{
   // Initialize the visited grid
   visited.resize(height);
   for (int i = 0; i < height; ++i) {
       visited[i].resize(width, false);
   }

   // Initialize the walls grid
    walls.resize(height);
    for (int i = 0; i < height; i++)
    {
        walls[i].resize(width);
        for (int j = 0; j < width; i++)
        {
            // All walls are initially present
            walls[i][j].resize(4, true);
        }
    }
}

// generate the maze using Depth-First Search Algorithm
void maze::generateMaze()
{
    // Stack to keep track of the cells
    stack<pair<int, int>> stack;

    // starting point at a random position 
    uniform_int_distribution<int> startX(0, width - 1);
    uniform_int_distribution<int> startY(0, height - 1);

    int startCellX = startX(rng);
    int startCellY = startY(rng);

    // Mark the starting cell as visited
    visited[startCellY][startCellX] = true;
    stack.push({ startCellX, startCellY });

    // Directions for moving in the maze (up, down, left, right)
    const vector<pair<int, int>> directions = {
        {0, -1},  // Up
        {0, 1},   // Down
        {-1, 0},  // Left
        {1, 0}    // Right
    };

    while (!stack.empty()){

        // Get the current cell
        auto [currentX, currentY] = stack.top();

        // Get the unvisited neighbors
        vector<pair<int, int>> neighbors;
        for (const auto& dir : directions) {
            int newX = currentX + dir.first;
            int newY = currentY + dir.second;

            // Check if the neighbor is within bounds and not visited
            if (newX >= 0 && newX < width && newY >= 0 && newY < height && !visited[newY][newX]) {
                neighbors.push_back({ newX, newY });
            }
        }
        
        // If there are unvisited neighbors, backtrack 
        if (neighbors.empty()){
          stack.pop();
          continue;
        }

        // chose a random neighbor
        uniform_int_distribution<int> randomNeighbor(0, neighbors.size() - 1);
        int randomIndex = randomNeighbor(rng);
        int direction = neighbors[randomIndex].first - currentX;

        // get the coordinates of the neighbor
        int neighborX = neighbors[randomIndex].first;
        int neighborY = neighbors[randomIndex].second;

        // Remove the wall between the current cell and the chosen neighbor
        // Remove the wall in the current cell
        walls[currentY][currentX][direction] = false;
        // Remove the wall in the neighbor cell
        walls[neighborY][neighborX][(direction + 2) % 4] = false;

        // Mark the neighbor as visited
        visited[neighborY][neighborX] = true;
        stack.push({ neighborX, neighborY });
    }
}

void maze::createWalls(const string &texturePath){

   // Create walls based on the walls grid
    float wallHeight = 1.0f; // Height of the wall
    float wallthickness = 0.5f; // Thickness of the wall

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Check if there is a wall in the current cell

            float x = position.x + (i * cellSize);
            float y = position.y;
            float z = position.z + (j * cellSize);

            if (walls[i][j][0]) { // Up in the current cell

                // Create a new wall object with the specified position and size and its texture in the constructor for the upper wall
               Wall* wall = new Wall(glm::vec3(position.x, position.y + wallHeight/2, position.z - cellSize), glm::vec3(cellSize, wallHeight, wallthickness), texturePath);
               //Add the wall to the wallObjects vector so that it can be rendered
               wallObjects.push_back(wall);
            }

            if (walls[i][j][1]){ // Down in the current cell
                // Create a new wall object with the specified position and size and its texture in the constructor for the lower wall
                Wall* wall = new Wall(glm::vec3(position.x, position.y + wallHeight/2, position.z + cellSize/2), glm::vec3(cellSize, wallHeight, wallthickness), texturePath);
                //Add the wall to the wallObjects vector so that it can be rendered
                wallObjects.push_back(wall);
            }
            
            if (walls [i][j][2]){ // Left in the current cell
                // Create a new wall object with the specified position and size and its texture in the constructor for the left wall
                Wall* wall = new Wall(glm::vec3(position.x + cellSize/2, position.y + wallHeight/2, position.z), glm::vec3(wallthickness, wallHeight, cellSize), texturePath);
                //Add the wall to the wallObjects vector so that it can be rendered
                wallObjects.push_back(wall);
            }

            if (walls[i][j][3]){ // Right in the current cell
                // Create a new wall object with the specified position and size and its texture in the constructor for the right wall
                Wall* wall = new Wall(glm::vec3(position.x - cellSize/2, position.y + wallHeight/2, position.z), glm::vec3(wallthickness, wallHeight, cellSize), texturePath);
                //Add the wall to the wallObjects vector so that it can be rendered
                wallObjects.push_back(wall);
            }
        }
    }

        // Add boundary walls to ensure the maze is enclosed
            //Create the top and bottom boundary walls
            for (int i = 0; i < width; i++){
             // top boundary wall
            if (walls[0][i][0]){
             Wall* wall = new Wall(glm::vec3(position.x + (i * cellSize), position.y + wallHeight/2, position.z), glm::vec3(cellSize, wallHeight, wallthickness), texturePath);
             wallObjects.push_back(wall);
             }
        // bottom boundary wall
        if (walls[height - 1][i][2]){
            Wall* wall = new Wall(glm::vec3(position.x + (i * cellSize), position.y + wallHeight/2, position.z + (height * cellSize)), glm::vec3(cellSize, wallHeight, wallthickness), texturePath);
            wallObjects.push_back(wall);
         }
    } 

    // Create the left and right boundary walls
    for (int i = 0; i < height; i++){
        // left boundary wall
        if (walls[i][0][3]){
            Wall* wall = new Wall(glm::vec3(position.x, position.y + wallHeight/2, position.z + (i * cellSize)), glm::vec3(wallthickness, wallHeight, cellSize), texturePath);
            wallObjects.push_back(wall);
        }
        // right boundary wall
        if (walls[i][width - 1][1]){
            Wall* wall = new Wall(glm::vec3(position.x + (width * cellSize), position.y + wallHeight/2, position.z + (i * cellSize)), glm::vec3(wallthickness, wallHeight, cellSize), texturePath);
            wallObjects.push_back(wall);
        }
    }
}

// check if the position collides with the maze
bool maze::checkCollision(const glm::vec3& position) const
{
    // Check if the position is within the maze boundaries
    if (position.x < this->position.x || position.x > this->position.x + (width * cellSize) ||
        position.z < this->position.z || position.z > this->position.z + (height * cellSize)) {
        return true; // Collision with the maze boundaries
    }

    // convert the position to maze coordinates
    int cellX = static_cast<int>((position.x - this->position.x) / cellSize);
    int cellZ = static_cast<int>((position.z - this->position.z) / cellSize);
    
    // get local position in the cell
    float localX = (position.x - this->position.x) - (cellX * cellSize);
    float localZ = (position.z - this->position.z) - (cellZ * cellSize);
 
    if (cellX < 0 || cellX >= width || cellZ < 0 || cellZ >= height) {
       //North wall
       if (walls[cellZ][cellX][0] && localZ < 0.5f) {
            return true; // Collision with the north wall
        }
        // South wall
        if (walls[cellZ][cellX][1] && localZ > cellSize - 0.5f) {
            return true; // Collision with the south wall
        }
        // West wall
        if (walls[cellZ][cellX][2] && localX < 0.5f) {
            return true; // Collision with the west wall
        }
        // East wall
        if (walls[cellZ][cellX][3] && localX > cellSize - 0.5f) {
            return true; // Collision with the east wall
        }
    }
    return false; // No collision
}

void maze::render(shaders* shader, const glm::mat4& view, const glm::mat4& projection)
{
    for (auto wall : wallObjects) {
        wall->render(shader, view, projection);
    }
}

glm::vec3 maze::getPosition() const {
    return glm :: vec3 (position.x + (width * cellSize), position.y + 0.1f, position.z + (height * cellSize));
}

glm::vec3 maze::getEndPosition() const {
    return glm::vec3(position.x + (width * cellSize), position.y + 0.1f, position.z + (height * cellSize));
}





