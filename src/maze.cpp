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
    createFloors("assets/FloorTiles/FloorTilesDeffuse.png"); // Add floor creation with tile texture
}

//destructor
maze::~maze()
{
   for (auto wall : wallObjects){
         delete wall;
   }
   wallObjects.clear();
   
   // Clean up floor objects
   for (auto floor : floorObjects){
         delete floor;
   }
   floorObjects.clear();
   
   // Clean up path objects
   for (auto path : pathObjects){
         delete path;
   }
   pathObjects.clear();
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
        for (int j = 0; j < width; j++)
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

    // Initialize the walls to be present
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < 4; k++) {
                //Set all the walls to true
                walls[i][j][k] = true; 
            }
        }
    }

    // Set a random starting point
    uniform_int_distribution<int> startX(0, width - 1);
    uniform_int_distribution<int> startY(0, height - 1);

    int startCellX = 0; // Start in top-left corner
    int startCellY = 0;

    // Mark the starting cell as visited
    visited[startCellY][startCellX] = true;
    stack.push({ startCellX, startCellY });
    
    // Keep track of how many cells have been visited
    int visitedCount = 1;
    int totalCells = width * height;
    
    // Direction mappings (N, S, W, E)
    const vector<pair<int, int>> directions = {
        {0, -1},  // North
        {0, 1},   // South
        {-1, 0},  // West
        {1, 0}    // East
    };
    
    // Direction to index mapping: N=0, S=1, W=2, E=3
    const vector<int> dirToIndex = {0, 1, 2, 3};

    // Keep carving passages while there are cells to visit
    while (!stack.empty()) {
        // Get the current cell
        auto [currentX, currentY] = stack.top();

        // Find all unvisited neighbors
        vector<pair<int, int>> unvisitedNeighbors;
        for (int d = 0; d < directions.size(); d++) {
            int nx = currentX + directions[d].first;
            int ny = currentY + directions[d].second;
            
            // Check if neighbor is valid and unvisited
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[ny][nx]) {
                unvisitedNeighbors.push_back({nx, ny});
            }
        }

        // If all neighbors are visited, backtrack
        if (unvisitedNeighbors.empty()) {
            stack.pop();
            continue;
        }
        
        // Choose a random unvisited neighbor
        uniform_int_distribution<int> randomNeighbor(0, unvisitedNeighbors.size() - 1);
        int randomIndex = randomNeighbor(rng);
        int nextX = unvisitedNeighbors[randomIndex].first;
        int nextY = unvisitedNeighbors[randomIndex].second;
        
        // Determine direction of movement
        int dx = nextX - currentX;
        int dy = nextY - currentY;
        
        // Remove walls between current and chosen cells
        if (dx == 1) {  // Moving East
            walls[currentY][currentX][3] = false;  // Remove East wall of current
            walls[nextY][nextX][2] = false;       // Remove West wall of neighbor
        }
        else if (dx == -1) {  // Moving West
            walls[currentY][currentX][2] = false;  // Remove West wall of current
            walls[nextY][nextX][3] = false;       // Remove East wall of neighbor
        }
        else if (dy == 1) {  // Moving South
            walls[currentY][currentX][1] = false;  // Remove South wall of current
            walls[nextY][nextX][0] = false;       // Remove North wall of neighbor
        }
        else if (dy == -1) {  // Moving North
            walls[currentY][currentX][0] = false;  // Remove North wall of current
            walls[nextY][nextX][1] = false;       // Remove South wall of neighbor
        }
        
        // Mark the neighbor as visited
        visited[nextY][nextX] = true;
        visitedCount++;
        
        // Push the neighbor to the stack
        stack.push({nextX, nextY});
    }

    // Randomly add back walls to make the maze more complex
    uniform_int_distribution<int> addBackWall(0, 4); 
    
    // Make sure entrance and exit are clear
    walls[0][0][0] = false; // Clear entrance
    walls[height-1][width-1][1] = false; // Clear exit
}


// create walls based on the walls grid
void maze::createWalls(const string &texturePath){

    float wallHeight = 2.0f; // Increase wall height for better visibility
    float wallThickness = 0.15f; // Wall thickness
    float overlap = 0.005f; // Tiny overlap to prevent gaps between walls

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Calculate the center position of the current cell
            float x = position.x + (j * cellSize) + cellSize/2;
            float y = position.y;
            float z = position.z + (i * cellSize) + cellSize/2;

            if (walls[i][j][0]) { // North wall
                Wall* wall = new Wall(
                    glm::vec3(x, position.y + wallHeight/2, z - cellSize/2 + wallThickness/2 - overlap),
                    glm::vec3(cellSize + overlap*2, wallHeight, wallThickness), 
                    texturePath
                );
                wallObjects.push_back(wall);
            }

            if (walls[i][j][1]){ // South wall
                Wall* wall = new Wall(
                    glm::vec3(x, position.y + wallHeight/2, z + cellSize/2 - wallThickness/2 + overlap),
                    glm::vec3(cellSize + overlap*2, wallHeight, wallThickness), 
                    texturePath
                );
                wallObjects.push_back(wall);
            }
            
            if (walls[i][j][2]){ // West wall
                Wall* wall = new Wall(
                    glm::vec3(x - cellSize/2 + wallThickness/2 - overlap, position.y + wallHeight/2, z),
                    glm::vec3(wallThickness, wallHeight, cellSize + overlap*2), 
                    texturePath
                );
                wallObjects.push_back(wall);
            }

            if (walls[i][j][3]){ // East wall
                Wall* wall = new Wall(
                    glm::vec3(x + cellSize/2 - wallThickness/2 + overlap, position.y + wallHeight/2, z),
                    glm::vec3(wallThickness, wallHeight, cellSize + overlap*2), 
                    texturePath
                );
                wallObjects.push_back(wall);
            }
        }
    }

    // Similar adjustments for boundary walls
    for (int j = 0; j < width; j++){
        float x = position.x + (j * cellSize) + cellSize/2;
        
        if (walls[0][j][0]){ // North boundary wall
            Wall* wall = new Wall(
                glm::vec3(x, position.y + wallHeight/2, position.z + overlap),
                glm::vec3(cellSize + overlap*2, wallHeight, wallThickness),
                texturePath
            );
            wallObjects.push_back(wall);
        }
        
        if (walls[height - 1][j][1]){ // South boundary wall
            Wall* wall = new Wall(
                glm::vec3(x, position.y + wallHeight/2, position.z + (height * cellSize) - overlap),
                glm::vec3(cellSize + overlap*2, wallHeight, wallThickness),
                texturePath
            );
            wallObjects.push_back(wall);
        }
    } 

    // Create the left and right boundary walls
    for (int i = 0; i < height; i++){
        float z = position.z + (i * cellSize) + cellSize/2;
        
        if (walls[i][0][2]){ // West boundary wall
            Wall* wall = new Wall(
                glm::vec3(position.x + overlap, position.y + wallHeight/2, z),
                glm::vec3(wallThickness, wallHeight, cellSize + overlap*2),
                texturePath
            );
            wallObjects.push_back(wall);
        }
        
        if (walls[i][width - 1][3]){ // East boundary wall
            Wall* wall = new Wall(
                glm::vec3(position.x + (width * cellSize) - overlap, position.y + wallHeight/2, z),
                glm::vec3(wallThickness, wallHeight, cellSize + overlap*2),
                texturePath
            );
            wallObjects.push_back(wall);
        }
    }
}

// Create the floor of the maze
void maze::createFloors(const string &floorTexturePath) {
    // Create a more visible floor with proper texture tiling
    // Create main floor for the entire maze with more visible texture
    Floor* mainFloor = new Floor(
        glm::vec3(position.x + (width * cellSize)/2, position.y - 0.01f, position.z + (height * cellSize)/2), // Center of the maze, slightly lower
        glm::vec2(width * cellSize, height * cellSize),                                               // Size of the entire maze
        floorTexturePath
    );
    floorObjects.push_back(mainFloor);
    
    // Add a final floor for the exit area
    Floor* exitFloor = new Floor(
        glm::vec3(position.x + (width * cellSize) + cellSize/2, position.y - 0.01f, position.z + (height * cellSize)/2), 
        glm::vec2(cellSize*2, cellSize*2), 
        floorTexturePath
    );
    floorObjects.push_back(exitFloor);
}

// check if the position collides with the maze
bool maze::checkCollision(const glm::vec3& position) const
{
    // Reduce collision buffer
    const float collisionBuffer = 0.12f;
    
    // Check if the position is outside the maze
    if (position.x < this->position.x + collisionBuffer || 
        position.x > this->position.x + (width * cellSize) - collisionBuffer ||
        position.z < this->position.z + collisionBuffer || 
        position.z > this->position.z + (height * cellSize) - collisionBuffer) {
        return true; // Collision with the maze boundaries
    }

    // Convert the position to maze coordinates - handle edge cases better
    int cellX = static_cast<int>((position.x - this->position.x) / cellSize);
    int cellZ = static_cast<int>((position.z - this->position.z) / cellSize);
    
    // Ensure we're within valid cell range
    if (cellX < 0) cellX = 0;
    if (cellX >= width) cellX = width - 1;
    if (cellZ < 0) cellZ = 0;
    if (cellZ >= height) cellZ = height - 1;
    
    // Get local position in the cell with better precision
    float localX = (position.x - this->position.x) - (cellX * cellSize);
    float localZ = (position.z - this->position.z) - (cellZ * cellSize);
 
    // Check collisions with current cell walls
    bool collision = false;
    
    // North wall collision
    if (walls[cellZ][cellX][0] && localZ < collisionBuffer) {
        collision = true;
    }
    // South wall collision
    else if (walls[cellZ][cellX][1] && localZ > cellSize - collisionBuffer) {
        collision = true;
    }
    // West wall collision
    else if (walls[cellZ][cellX][2] && localX < collisionBuffer) {
        collision = true;
    }
    // East wall collision
    else if (walls[cellZ][cellX][3] && localX > cellSize - collisionBuffer) {
        collision = true;
    }
    
    // Check for corner cases - when we're near cell boundaries
    // This prevents getting stuck at cell corners
    if (localX < collisionBuffer && localZ < collisionBuffer) {
        // Near northwest corner - check diagonal cell if it exists
        if (cellX > 0 && cellZ > 0) {
            if (walls[cellZ-1][cellX-1][1] || walls[cellZ-1][cellX-1][3]) {
                collision = true;
            }
        }
    }
    else if (localX > cellSize - collisionBuffer && localZ < collisionBuffer) {
        // Near northeast corner
        if (cellX < width-1 && cellZ > 0) {
            if (walls[cellZ-1][cellX+1][1] || walls[cellZ-1][cellX+1][2]) {
                collision = true;
            }
        }
    }
    
    return collision;
}


// Render the maze
void maze::render(shaders* shader, const glm::mat4& view, const glm::mat4& projection)
{
    // Render all floor tiles first (so they appear below everything)
    for (auto floor : floorObjects) {
        floor->render(shader, view, projection);
    }
    
    // Render path markers above the floor but below walls
    for (auto path : pathObjects) {
        path->render(shader, view, projection);
    }
    
    // Then render all walls
    for (auto wall : wallObjects) {
        wall->render(shader, view, projection);
    }
}



// Create visual markers for the path
void maze::createPathMarkers() {
    for (const auto& cell : pathCells) {
        int x = cell.first;
        int y = cell.second;
        
        // Calculate position for this path marker
        float px = position.x + (x * cellSize) + cellSize/2;
        float py = position.y + 0.02f;  // Slightly higher above floor for better visibility
        float pz = position.z + (y * cellSize) + cellSize/2;
        
        // Create a new floor object with a different color/texture for the path
        Floor* pathMarker = new Floor(
            glm::vec3(px, py, pz),                    // Centered in the cell
            glm::vec2(cellSize * 0.5f, cellSize * 0.5f),  // Make path markers visible but not too large
            "assets/FloorTiles/FloorTilesSpacular.png"    // Use a different texture for path
        );
        
        // Add the path marker to the pathObjects vector
        pathObjects.push_back(pathMarker);
    }
    
    // Create a special marker for the goal/exit
    if (!pathCells.empty()) {
        // Get the end position (exit)
        float exitX = position.x + (width-1) * cellSize + cellSize/2;
        float exitY = position.y + 0.03f; // Slightly higher than path markers
        float exitZ = position.z + (height-1) * cellSize + cellSize/2;
        
        // Create a prominent marker for the exit
        Floor* exitMarker = new Floor(
            glm::vec3(exitX, exitY, exitZ),
            glm::vec2(cellSize * 0.7f, cellSize * 0.7f), // Larger than path markers
            "assets/FloorTiles/FloorTilesNormal.png"  // Different texture for the exit
        );
        
        pathObjects.push_back(exitMarker);
    }
}

// Generate a path from start to end with randomized depth-first search algorithm
void maze::generatePath() {
    // Clear any existing path
    pathCells.clear();
    for (auto path : pathObjects) {
        delete path;
    }
    pathObjects.clear();
    
    // Get start and end cell indices
    int startX = 0;
    int startY = 0;
    int endX = width - 1;
    int endY = height - 1;
    
    // Reset visited cells for pathfinding
    vector<vector<bool>> pathVisited(height, vector<bool>(width, false));
    
    // Create a stack for the depth-first search for the floor
    stack<pair<int, int>> pathStack;
    
    // Create a map to store the path from start to end
    vector<vector<pair<int, int>>> parent(height, vector<pair<int, int>>(width, {-1, -1}));
    
    // Start DFS from the start cell
    pathVisited[startY][startX] = true;
    pathStack.push({startX, startY});
    
    // Define directions (N, S, W, E)
    const int numDirections = 4;
    const int dx[numDirections] = {0, 0, -1, 1};  // x-offset for N, S, W, E
    const int dy[numDirections] = {-1, 1, 0, 0};  // y-offset for N, S, W, E
    
    bool foundPath = false;
    
    // Randomized DFS to find a path
    while (!pathStack.empty() && !foundPath) {
        // Get the current cell
        auto current = pathStack.top();
        int x = current.first;
        int y = current.second;
        pathStack.pop();
        
        // Check if we've reached the end
        if (x == endX && y == endY) {
            foundPath = true;
            break;
        }
        
        // Get all valid neighbors
        vector<pair<int, int>> neighbors;

        for (int dir = 0; dir < numDirections; dir++) {
            // Skip if there's a wall in this direction
            if (walls[y][x][dir]) {
                continue;
            }
            
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            
            // Check if neighbor is valid and not visited
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && !pathVisited[ny][nx]) {
                neighbors.push_back({nx, ny});
            }
        }
        
        // If we have unvisited neighbors, choose one randomly
        if (!neighbors.empty()) {
            // Randomly shuffle the neighbors to ensure randomness
            std::shuffle(neighbors.begin(), neighbors.end(), rng);
            
            for (const auto& neighbor : neighbors) {
                int nx = neighbor.first;
                int ny = neighbor.second;
                pathVisited[ny][nx] = true;
                parent[ny][nx] = {x, y};
                pathStack.push({nx, ny});
            }
        }
    }
    
    // If we found a path, trace back from end to start
    if (foundPath) {
        int x = endX;
        int y = endY;
        
        while (!(x == startX && y == startY)) {
            pathCells.push_back({x, y});
            auto parent_cell = parent[y][x];
            x = parent_cell.first;
            y = parent_cell.second;
        }
        
        // Add the start cell
        pathCells.push_back({startX, startY});
        
        // Reverse path to go from start to end
        std::reverse(pathCells.begin(), pathCells.end());
        
        // Create visual markers for the path
        createPathMarkers();
        
        cout << "Random path generated with " << pathCells.size() << " cells" << endl;
    } else {
        cout << "No path found from start to end" << endl;
        
        // Create a direct path by breaking walls
        createDirectPath(startX, startY, endX, endY);
        
        // Try path generation again
        generatePath();
    }
}

// Helper function to create a direct path by breaking walls
void maze::createDirectPath(int startX, int startY, int endX, int endY) {
    int x = startX;
    int y = startY;
    
    // Move horizontally first
    while (x < endX) {
        // Remove the east wall of the current cell
        walls[y][x][3] = false;
        // Remove the west wall of the next cell
        walls[y][x+1][2] = false;
        x++;
    }
    
    // Then move vertically
    while (y < endY) {
        // Remove the south wall of the current cell
        walls[y][x][1] = false;
        // Remove the north wall of the cell below
        walls[y+1][x][0] = false;
        y++;
    }
}

// Return the starting position of the maze
glm::vec3 maze::getPosition() const {
    // Return the position with a small offset for better player positioning
    return glm::vec3(position.x + cellSize/2, position.y + 0.5f, position.z + cellSize/2);
}

// Return the ending position of the maze
glm::vec3 maze::getEndPosition() const {
    // Return the position of the bottom-right corner cell
    return glm::vec3(
        position.x + (width-1) * cellSize + cellSize/2,
        position.y + 0.5f,
        position.z + (height-1) * cellSize + cellSize/2
    );
}





