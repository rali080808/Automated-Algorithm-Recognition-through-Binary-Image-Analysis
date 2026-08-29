#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

class SudokuSolver {
private:
    vector<vector<int>> board;
    int N; // Board size (N×N)
    int SQN; // Square root of N
    int solutions; // Count of solutions
    bool findAllSolutions; // Flag to find all solutions

    // Check if a number can be placed at a position
    bool isSafe(int row, int col, int num) {
        // Check row and column
        for (int i = 0; i < N; i++) {
            if (board[row][i] == num || board[i][col] == num) {
                return false;
            }
        }

        // Check box
        int boxRowStart = row - row % SQN;
        int boxColStart = col - col % SQN;

        for (int i = 0; i < SQN; i++) {
            for (int j = 0; j < SQN; j++) {
                if (board[boxRowStart + i][boxColStart + j] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    // DFS-based sudoku solver
    bool solveSudokuDFS() {
        // Find an empty cell
        int row = -1, col = -1;
        bool isEmpty = false;

        for (int i = 0; i < N && !isEmpty; i++) {
            for (int j = 0; j < N && !isEmpty; j++) {
                if (board[i][j] == 0) {
                    row = i;
                    col = j;
                    isEmpty = true;
                }
            }
        }

        // No empty cells left, puzzle is solved
        if (!isEmpty) {
            solutions++;
            return true;
        }

        // Try different numbers in the empty cell
        for (int num = 1; num <= N; num++) {
            if (isSafe(row, col, num)) {
                // Place the number
                board[row][col] = num;

                // Recur to solve rest of the board
                if (solveSudokuDFS()) {
                    // If we found a solution and aren't looking for all solutions, return true
                    if (!findAllSolutions) {
                        return true;
                    }
                    // Otherwise, continue looking for more solutions
                }

                // If no solution or finding all solutions, backtrack
                board[row][col] = 0;
            }
        }

        // No solution found
        return false;
    }

public:
    SudokuSolver(const vector<vector<int>>& initialBoard, bool findAll = false) {
        board = initialBoard;
        N = board.size();
        SQN = sqrt(N);
        solutions = 0;
        findAllSolutions = findAll;
    }

    // Solve the Sudoku puzzle
    bool solve() {
        solutions = 0;
        bool result = solveSudokuDFS();
        return result;
    }

    // Get number of solutions found
    int getSolutionCount() const {
        return solutions;
    }

    // Get the solved board
    vector<vector<int>> getSolution() const {
        return board;
    }

    // Print the board
    void printBoard() const {
        int width = 2; // Width for cell display

        for (int i = 0; i < N; i++) {
            if (i % SQN == 0 && i != 0) {
                for (int j = 0; j < N*width + N/SQN; j++) {
                    cout << "-";
                }
                cout << endl;
            }

            for (int j = 0; j < N; j++) {
                if (j % SQN == 0 && j != 0) {
                    cout << "| ";
                }

                if (board[i][j] == 0) {
                    cout << ". ";
                } else {
                    cout << board[i][j] << " ";
                }
            }
            cout << endl;
        }
    }

    // Check if a board is valid
    bool isValidBoard() {
        // Check each cell
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] != 0) {
                    // Store the value
                    int num = board[i][j];

                    // Clear the cell temporarily
                    board[i][j] = 0;

                    // Check if the value is valid
                    if (!isSafe(i, j, num)) {
                        // Restore the value
                        board[i][j] = num;
                        return false;
                    }

                    // Restore the value
                    board[i][j] = num;
                }
            }
        }

        return true;
    }
};

int main() {
    // Example 1: 9×9 Sudoku
    vector<vector<int>> board = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    SudokuSolver solver(board);

    cout << "Original Sudoku:" << endl;
    solver.printBoard();

    if (solver.solve()) {
        cout << "\nSolved Sudoku:" << endl;
        solver.printBoard();
    } else {
        cout << "\nNo solution exists." << endl;
    }

    // Example 2: 4×4 Sudoku with multiple solutions
    vector<vector<int>> smallBoard = {
        {0, 0, 1, 2},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {2, 1, 0, 0}
    };

    SudokuSolver multiSolver(smallBoard, true);

    cout << "\nOriginal 4x4 Sudoku:" << endl;
    multiSolver.printBoard();

    if (multiSolver.solve()) {
        cout << "\nOne solution for 4x4 Sudoku:" << endl;
        multiSolver.printBoard();
        cout << "Total solutions found: " << multiSolver.getSolutionCount() << endl;
    } else {
        cout << "\nNo solution exists." << endl;
    }

    // Example 3: Invalid Sudoku
    vector<vector<int>> invalidBoard = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 5, 3, 0, 0, 1}, // Note that 5 is duplicated in this row
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    SudokuSolver invalidSolver(invalidBoard);

    cout << "\nInvalid Sudoku:" << endl;
    invalidSolver.printBoard();

    if (invalidSolver.isValidBoard()) {
        cout << "Sudoku board is valid." << endl;

        if (invalidSolver.solve()) {
            cout << "\nSolved Sudoku:" << endl;
            invalidSolver.printBoard();
        } else {
            cout << "\nNo solution exists." << endl;
        }
    } else {
        cout << "Sudoku board is invalid." << endl;
    }

    return 0;
}
