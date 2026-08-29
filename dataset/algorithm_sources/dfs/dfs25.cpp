#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class CombinatorialSolver {
private:
    // N-Queens: Check if a queen can be placed at board[row][col]
    bool isSafeQueens(const vector<vector<char>>& board, int row, int col, int n) {
        // Check row (left side, as we're filling from left to right)
        for (int i = 0; i < col; i++) {
            if (board[row][i] == 'Q') return false;
        }

        // Check upper diagonal
        for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) {
            if (board[i][j] == 'Q') return false;
        }

        // Check lower diagonal
        for (int i = row, j = col; i < n && j >= 0; i++, j--) {
            if (board[i][j] == 'Q') return false;
        }

        return true;
    }

    // N-Queens solver using DFS backtracking
    bool solveNQueensUtil(vector<vector<char>>& board, int col, int n, int& solutionCount) {
        // Base case: all queens are placed
        if (col >= n) {
            solutionCount++;
            return true;
        }

        bool foundSolution = false;

        // Try placing a queen in each row of this column
        for (int row = 0; row < n; row++) {
            // Check if the queen can be placed
            if (isSafeQueens(board, row, col, n)) {
                // Place the queen
                board[row][col] = 'Q';

                // Recur to place rest of the queens
                bool success = solveNQueensUtil(board, col + 1, n, solutionCount);

                // If we want all solutions, don't return yet
                if (success) {
                    foundSolution = true;
                    // If we only want one solution, return immediately
                    if (!findAllSolutions) {
                        return true;
                    }
                }

                // Backtrack
                board[row][col] = '.';
            }
        }

        return foundSolution;
    }

    // Check if knight's tour position is valid and unvisited
    bool isValidKnightMove(const vector<vector<int>>& board, int x, int y, int n) {
        return (x >= 0 && x < n && y >= 0 && y < n && board[x][y] == -1);
    }

    // Knight's Tour solver using DFS backtracking
    bool solveKnightTourUtil(vector<vector<int>>& board, int x, int y, int moveNum, int n,
                            vector<pair<int, int>>& moves) {
        // Base case: all squares are visited
        if (moveNum == n * n) {
            return true;
        }

        // Try all possible moves from current position
        for (const auto& move : moves) {
            int nextX = x + move.first;
            int nextY = y + move.second;

            if (isValidKnightMove(board, nextX, nextY, n)) {
                // Make the move
                board[nextX][nextY] = moveNum;

                // Recur to solve rest of the tour
                if (solveKnightTourUtil(board, nextX, nextY, moveNum + 1, n, moves)) {
                    return true;
                }

                // Backtrack
                board[nextX][nextY] = -1;
            }
        }

        return false;
    }

    // Check if a letter can be placed at position (row, col) in the crossword
    bool canPlaceLetter(vector<vector<char>>& grid, int row, int col,
                      char letter, int rows, int cols) {
        // Out of bounds
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            return false;
        }

        // Cell is blocked or already has a different letter
        if (grid[row][col] == '#' || (grid[row][col] != '.' && grid[row][col] != letter)) {
            return false;
        }

        return true;
    }

    // Check if we can place a word horizontally
    bool canPlaceHorizontally(vector<vector<char>>& grid, int row, int col,
                           const string& word, int rows, int cols) {
        // If the word doesn't fit
        if (col + word.length() > cols) {
            return false;
        }

        // Check if cell to the left is empty
        if (col > 0 && grid[row][col - 1] != '#') {
            return false;
        }

        // Check if cell to the right is empty
        if (col + word.length() < cols && grid[row][col + word.length()] != '#') {
            return false;
        }

        // Check each cell
        for (int i = 0; i < word.length(); i++) {
            if (!canPlaceLetter(grid, row, col + i, word[i], rows, cols)) {
                return false;
            }
        }

        return true;
    }

    // Check if we can place a word vertically
    bool canPlaceVertically(vector<vector<char>>& grid, int row, int col,
                         const string& word, int rows, int cols) {
        // If the word doesn't fit
        if (row + word.length() > rows) {
            return false;
        }

        // Check if cell above is empty
        if (row > 0 && grid[row - 1][col] != '#') {
            return false;
        }

        // Check if cell below is empty
        if (row + word.length() < rows && grid[row + word.length()][col] != '#') {
            return false;
        }

        // Check each cell
        for (int i = 0; i < word.length(); i++) {
            if (!canPlaceLetter(grid, row + i, col, word[i], rows, cols)) {
                return false;
            }
        }

        return true;
    }

    // Place a word horizontally
    void placeWordHorizontally(vector<vector<char>>& grid, int row, int col,
                             const string& word) {
        for (int i = 0; i < word.length(); i++) {
            grid[row][col + i] = word[i];
        }
    }

    // Place a word vertically
    void placeWordVertically(vector<vector<char>>& grid, int row, int col,
                           const string& word) {
        for (int i = 0; i < word.length(); i++) {
            grid[row + i][col] = word[i];
        }
    }

    // Remove a word horizontally
    void removeWordHorizontally(vector<vector<char>>& grid, int row, int col,
                              const string& word, const vector<vector<char>>& original) {
        for (int i = 0; i < word.length(); i++) {
            grid[row][col + i] = original[row][col + i];
        }
    }

    // Remove a word vertically
    void removeWordVertically(vector<vector<char>>& grid, int row, int col,
                            const string& word, const vector<vector<char>>& original) {
        for (int i = 0; i < word.length(); i++) {
            grid[row + i][col] = original[row + i][col];
        }
    }

    // Crossword puzzle solver using DFS backtracking
    bool solveCrosswordUtil(vector<vector<char>>& grid, const vector<string>& words,
                          int wordIndex, int rows, int cols, const vector<vector<char>>& original) {
        // Base case: all words are placed
        if (wordIndex == words.size()) {
            return true;
        }

        const string& currentWord = words[wordIndex];

        // Try placing the word at each position
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                // Try placing horizontally
                if (canPlaceHorizontally(grid, row, col, currentWord, rows, cols)) {
                    placeWordHorizontally(grid, row, col, currentWord);

                    if (solveCrosswordUtil(grid, words, wordIndex + 1, rows, cols, original)) {
                        return true;
                    }

                    removeWordHorizontally(grid, row, col, currentWord, original);
                }

                // Try placing vertically
                if (canPlaceVertically(grid, row, col, currentWord, rows, cols)) {
                    placeWordVertically(grid, row, col, currentWord);

                    if (solveCrosswordUtil(grid, words, wordIndex + 1, rows, cols, original)) {
                        return true;
                    }

                    removeWordVertically(grid, row, col, currentWord, original);
                }
            }
        }

        return false;
    }

    // Generate all permutations of a string using DFS
    void generatePermutationsUtil(string& str, int left, int right, vector<string>& permutations) {
        // Base case: one permutation is complete
        if (left == right) {
            permutations.push_back(str);
        } else {
            // Try all characters as the next character
            for (int i = left; i <= right; i++) {
                // Check for duplicates to avoid generating duplicate permutations
                bool isDuplicate = false;
                for (int j = left; j < i; j++) {
                    if (str[j] == str[i]) {
                        isDuplicate = true;
                        break;
                    }
                }

                if (isDuplicate) continue;

                // Swap the characters
                swap(str[left], str[i]);

                // Recur with the next position
                generatePermutationsUtil(str, left + 1, right, permutations);

                // Backtrack
                swap(str[left], str[i]);
            }
        }
    }

    // Generate all subsets of a set using DFS
    void generateSubsetsUtil(const vector<int>& nums, int index, vector<int>& current,
                           vector<vector<int>>& subsets) {
        // Base case: all elements have been considered
        if (index == nums.size()) {
            subsets.push_back(current);
            return;
        }

        // Include the current element
        current.push_back(nums[index]);
        generateSubsetsUtil(nums, index + 1, current, subsets);

        // Exclude the current element (backtrack)
        current.pop_back();
        generateSubsetsUtil(nums, index + 1, current, subsets);
    }

    // Flag to determine whether to find all solutions or just one
    bool findAllSolutions;

public:
    CombinatorialSolver(bool findAll = false) : findAllSolutions(findAll) {}

    // Solve the N-Queens problem
    vector<vector<char>> solveNQueens(int n) {
        vector<vector<char>> board(n, vector<char>(n, '.'));
        int solutionCount = 0;

        if (solveNQueensUtil(board, 0, n, solutionCount)) {
            cout << "Found " << solutionCount << " solution(s) for " << n << "-Queens." << endl;
            return board;
        }

        cout << "No solution found for " << n << "-Queens." << endl;
        return {};
    }

    // Solve the Knight's Tour problem
    vector<vector<int>> solveKnightTour(int n, int startX = 0, int startY = 0) {
        vector<vector<int>> board(n, vector<int>(n, -1));

        // Possible moves for the knight
        vector<pair<int, int>> moves = {
            {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
            {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
        };

        // Start the tour
        board[startX][startY] = 0;

        if (solveKnightTourUtil(board, startX, startY, 1, n, moves)) {
            cout << "Knight's Tour solution found for " << n << "x" << n << " board." << endl;
            return board;
        }

        cout << "No Knight's Tour solution found for " << n << "x" << n << " board." << endl;
        return {};
    }

    // Solve a Crossword puzzle
    vector<vector<char>> solveCrossword(vector<vector<char>> grid, const vector<string>& words) {
        int rows = grid.size();
        int cols = grid[0].size();

        // Save the original grid for backtracking
        vector<vector<char>> original = grid;

        if (solveCrosswordUtil(grid, words, 0, rows, cols, original)) {
            cout << "Crossword puzzle solved successfully." << endl;
            return grid;
        }

        cout << "No solution found for the crossword puzzle." << endl;
        return {};
    }

    // Generate all permutations of a string
    vector<string> generatePermutations(const string& str) {
        vector<string> permutations;
        string s = str;
        generatePermutationsUtil(s, 0, s.length() - 1, permutations);
        return permutations;
    }

    // Generate all subsets of a set
    vector<vector<int>> generateSubsets(const vector<int>& nums) {
        vector<vector<int>> subsets;
        vector<int> current;
        generateSubsetsUtil(nums, 0, current, subsets);
        return subsets;
    }

    // Set option to find all solutions
    void setFindAllSolutions(bool findAll) {
        findAllSolutions = findAll;
    }
};

// Print a 2D character grid
void printGrid(const vector<vector<char>>& grid) {
    for (const auto& row : grid) {
        for (char cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
}

// Print a 2D integer grid
void printGrid(const vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            cout << cell << "\t";
        }
        cout << endl;
    }
}

// Print a vector of strings
void printStrings(const vector<string>& strings) {
    for (const string& s : strings) {
        cout << s << endl;
    }
}

// Print a vector of vectors
void printSubsets(const vector<vector<int>>& subsets) {
    for (const auto& subset : subsets) {
        cout << "{ ";
        for (int num : subset) {
            cout << num << " ";
        }
        cout << "}" << endl;
    }
}

int main() {
    CombinatorialSolver solver;

    // N-Queens Problem
    cout << "Solving 4-Queens Problem:" << endl;
    vector<vector<char>> queensSolution = solver.solveNQueens(4);
    if (!queensSolution.empty()) {
        printGrid(queensSolution);
    }
    cout << endl;

    // Knight's Tour Problem
    cout << "Solving Knight's Tour on a 5x5 board:" << endl;
    vector<vector<int>> knightTour = solver.solveKnightTour(5);
    if (!knightTour.empty()) {
        printGrid(knightTour);
    }
    cout << endl;

    // Crossword Puzzle
    cout << "Solving Crossword Puzzle:" << endl;
    vector<vector<char>> crossword = {
        {'.', '.', '#', '#', '.'},
        {'.', '.', '.', '#', '.'},
        {'.', '.', '.', '.', '.'},
        {'#', '.', '.', '.', '.'},
        {'.', '#', '.', '.', '.'}
    };
    vector<string> words = {"CAT", "DOG", "RAT", "BEE", "APE"};

    cout << "Original Crossword:" << endl;
    printGrid(crossword);
    cout << "Words to place: ";
    for (const string& word : words) {
        cout << word << " ";
    }
    cout << endl << endl;

    vector<vector<char>> crosswordSolution = solver.solveCrossword(crossword, words);
    if (!crosswordSolution.empty()) {
        cout << "Crossword Solution:" << endl;
        printGrid(crosswordSolution);
    }
    cout << endl;

    // String Permutations
    cout << "Generating permutations of 'ABC':" << endl;
    vector<string> permutations = solver.generatePermutations("ABC");
    printStrings(permutations);
    cout << endl;

    // Generating subsets
    cout << "Generating all subsets of {1, 2, 3}:" << endl;
    vector<int> nums = {1, 2, 3};
    vector<vector<int>> subsets = solver.generateSubsets(nums);
    printSubsets(subsets);

    return 0;
}
