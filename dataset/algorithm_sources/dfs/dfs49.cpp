#include <iostream>
#include <vector>
#include <cmath>

class SudokuSolver {
private:
    std::vector<std::vector<int>> board;
    int N; // Size of the Sudoku grid
    int SQN; // Square root of N

public:
    SudokuSolver(const std::vector<std::vector<int>>& initialBoard) {
        board = initialBoard;
        N = board.size();
        SQN = sqrt(N);
    }

    bool solve() {
        int row, col;

        // If there is no unassigned location, we are done
        if (!findEmptyLocation(row, col)) {
            return true; // Success!
        }

        // Try digits 1 to N
        for (int num = 1; num <= N; num++) {
            // Check if it's safe to place the number
            if (isSafe(row, col, num)) {
                // Make tentative assignment
                board[row][col] = num;

                // Return if success
                if (solve()) {
                    return true;
                }

                // Failure, unmake & try again
                board[row][col] = 0;
            }
        }

        // Trigger backtracking
        return false;
    }

    void printBoard() {
        for (int i = 0; i < N; i++) {
            if (i > 0 && i % SQN == 0) {
                for (int j = 0; j < N; j++) {
                    std::cout << "----";
                }
                std::cout << std::endl;
            }

            for (int j = 0; j < N; j++) {
                if (j > 0 && j % SQN == 0) {
                    std::cout << "| ";
                }

                if (board[i][j] == 0) {
                    std::cout << ". ";
                } else {
                    std::cout << board[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }

private:
    bool findEmptyLocation(int& row, int& col) {
        for (row = 0; row < N; row++) {
            for (col = 0; col < N; col++) {
                if (board[row][col] == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    bool isSafe(int row, int col, int num) {
        return !usedInRow(row, num) &&
               !usedInCol(col, num) &&
               !usedInBox(row - row % SQN, col - col % SQN, num);
    }

    bool usedInRow(int row, int num) {
        for (int col = 0; col < N; col++) {
            if (board[row][col] == num) {
                return true;
            }
        }
        return false;
    }

    bool usedInCol(int col, int num) {
        for (int row = 0; row < N; row++) {
            if (board[row][col] == num) {
                return true;
            }
        }
        return false;
    }

    bool usedInBox(int boxStartRow, int boxStartCol, int num) {
        for (int row = 0; row < SQN; row++) {
            for (int col = 0; col < SQN; col++) {
                if (board[row + boxStartRow][col + boxStartCol] == num) {
                    return true;
                }
            }
        }
        return false;
    }
};

int main() {
    // Example 9x9 Sudoku puzzle
    std::vector<std::vector<int>> board = {
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

    std::cout << "Sudoku Puzzle:" << std::endl;
    solver.printBoard();

    if (solver.solve()) {
        std::cout << "\nSolution:" << std::endl;
        solver.printBoard();
    } else {
        std::cout << "\nNo solution exists." << std::endl;
    }

    return 0;
}
