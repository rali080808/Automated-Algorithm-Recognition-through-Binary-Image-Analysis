#include <iostream>
#include <vector>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace std;

// Maze cell states
enum CellState {
    WALL = '#',
    PATH = ' ',
    START = 'S',
    END = 'E',
    VISITED = '.',
    SOLUTION = '*'
};

// Position in the maze
struct Position {
    int row, col;

    Position(int r = 0, int c = 0) : row(r), col(c) {}

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    // Get adjacent positions (up, right, down, left)
    vector<Position> getAdjacent() const {
        return {
            Position(row-1, col),
            Position(row, col+1),
            Position(row+1, col),
            Position(row, col-1)
        };
    }
};

class Maze {
private:
    vector<vector<char>> grid;
    int rows, cols;
    Position start, end;

    // Check if position is valid and traversable
    bool isValid(const Position& pos) const {
        return pos.row >= 0 && pos.row < rows &&
               pos.col >= 0 && pos.col < cols &&
               (grid[pos.row][pos.col] == PATH ||
                grid[pos.row][pos.col] == END);
    }

    // DFS with backtracking to solve the maze
    bool solveDFSUtil(const Position& current, vector<vector<bool>>& visited,
                      vector<Position>& path) {
        // Mark current cell as visited
        visited[current.row][current.col] = true;
        path.push_back(current);

        // If we reached the end
        if (current == end) {
            return true;
        }

        // Try all four directions
        for (const Position& next : current.getAdjacent()) {
            if (isValid(next) && !visited[next.row][next.col]) {
                if (solveDFSUtil(next, visited, path)) {
                    return true;
                }
            }
        }

        // Backtrack if no solution from this cell
        path.pop_back();
        return false;
    }

    // Iterative DFS to solve the maze
    vector<Position> solveIterativeDFS() {
        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        stack<Position> dfsStack;
        vector<vector<Position>> parent(rows, vector<Position>(cols, Position(-1, -1)));

        dfsStack.push(start);
        visited[start.row][start.col] = true;

        bool found = false;
        while (!dfsStack.empty() && !found) {
            Position current = dfsStack.top();
            dfsStack.pop();

            if (current == end) {
                found = true;
                break;
            }

            for (const Position& next : current.getAdjacent()) {
                if (isValid(next) && !visited[next.row][next.col]) {
                    dfsStack.push(next);
                    visited[next.row][next.col] = true;
                    parent[next.row][next.col] = current;
                }
            }
        }

        // Reconstruct the path if found
        vector<Position> path;
        if (found) {
            Position current = end;
            while (!(current == start)) {
                path.push_back(current);
                current = parent[current.row][current.col];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
        }

        return path;
    }

    // Generate a random maze using DFS
    void generateMazeDFS() {
        // Fill with walls
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j] = WALL;
            }
        }

        // Start with a random cell
        srand(time(nullptr));
        int startRow = rand() % (rows-2) + 1;
        int startCol = rand() % (cols-2) + 1;
        grid[startRow][startCol] = PATH;

        // Use DFS to carve paths
        vector<Position> frontiers;
        addFrontiers(Position(startRow, startCol), frontiers);

        while (!frontiers.empty()) {
            // Pick a random frontier
            int idx = rand() % frontiers.size();
            Position current = frontiers[idx];
            frontiers[idx] = frontiers.back();
            frontiers.pop_back();

            // Find connected cells that are already PATH
            vector<Position> connectedCells;
            for (const Position& pos : current.getAdjacent()) {
                if (pos.row >= 0 && pos.row < rows &&
                    pos.col >= 0 && pos.col < cols &&
                    grid[pos.row][pos.col] == PATH) {
                    connectedCells.push_back(pos);
                }
            }

            if (!connectedCells.empty()) {
                // Connect to a random connected cell
                int randIdx = rand() % connectedCells.size();
                Position connectTo = connectedCells[randIdx];

                // Carve a path
                grid[current.row][current.col] = PATH;

                // Add new frontiers
                addFrontiers(current, frontiers);
            }
        }

        // Place start and end points
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] == PATH) {
                    start = Position(i, j);
                    grid[i][j] = START;
                    break;
                }
            }
            if (grid[start.row][start.col] == START)
                break;
        }

        for (int i = rows-1; i >= 0; i--) {
            for (int j = cols-1; j >= 0; j--) {
                if (grid[i][j] == PATH) {
                    end = Position(i, j);
                    grid[i][j] = END;
                    break;
                }
            }
            if (grid[end.row][end.col] == END)
                break;
        }
    }

    // Add frontier cells for maze generation
    void addFrontiers(const Position& pos, vector<Position>& frontiers) {
        for (int dr = -2; dr <= 2; dr += 2) {
            for (int dc = -2; dc <= 2; dc += 2) {
                // Skip diagonals
                if (abs(dr) == abs(dc)) continue;

                Position frontier(pos.row + dr, pos.col + dc);
                if (frontier.row >= 0 && frontier.row < rows &&
                    frontier.col >= 0 && frontier.col < cols &&
                    grid[frontier.row][frontier.col] == WALL) {
                    frontiers.push_back(frontier);
                }
            }
        }
    }

public:
    Maze(int r, int c) : rows(r), cols(c) {
        grid.resize(rows, vector<char>(cols, PATH));
    }

    // Create a maze from a 2D grid
    Maze(const vector<vector<char>>& mazeGrid) : grid(mazeGrid) {
        rows = grid.size();
        cols = grid[0].size();

        // Find start and end positions
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] == START) {
                    start = Position(i, j);
                }
                else if (grid[i][j] == END) {
                    end = Position(i, j);
                }
            }
        }
    }

    // Generate a random maze
    void generateMaze() {
        generateMazeDFS();
    }

    // Solve the maze using recursive DFS with backtracking
    vector<Position> solveDFS() {
        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        vector<Position> path;

        solveDFSUtil(start, visited, path);
        return path;
    }

    // Print the maze
    void printMaze() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << grid[i][j];
            }
            cout << endl;
        }
    }

    // Print the maze with the solution path
    void printSolution(const vector<Position>& solution) {
        // Create a copy of the maze
        vector<vector<char>> solutionGrid = grid;

        // Mark the solution path
        for (const Position& pos : solution) {
            if (solutionGrid[pos.row][pos.col] != START &&
                solutionGrid[pos.row][pos.col] != END) {
                solutionGrid[pos.row][pos.col] = SOLUTION;
            }
        }

        // Print the maze with the solution
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << solutionGrid[i][j];
            }
            cout << endl;
        }

        cout << "Path length: " << solution.size() << endl;
    }

    // Set a specific cell
    void setCell(int row, int col, char value) {
        if (row >= 0 && row < rows && col >= 0 && col < cols) {
            grid[row][col] = value;

            if (value == START) {
                start = Position(row, col);
            }
            else if (value == END) {
                end = Position(row, col);
            }
        }
    }
};

int main() {
    // Example 1: Solve a predefined maze
    vector<vector<char>> mazeData = {
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', 'S', ' ', '#', ' ', ' ', ' ', '#', ' ', '#'},
        {'#', '#', ' ', '#', ' ', '#', ' ', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', '#', '#', '#', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
    };

    Maze maze1(mazeData);
    cout << "Original maze:" << endl;
    maze1.printMaze();

    vector<Position> solution1 = maze1.solveDFS();

    cout << "\nMaze solution:" << endl;
    maze1.printSolution(solution1);

    // Example 2: Create and solve a random maze
    cout << "\nRandom Maze:" << endl;
    Maze maze2(15, 25);
    maze2.generateMaze();
    maze2.printMaze();

    vector<Position> solution2 = maze2.solveDFS();

    cout << "\nRandom maze solution:" << endl;
    maze2.printSolution(solution2);

    return 0;
}
