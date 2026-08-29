// Example 4: DFS for solving the Knight's Tour problem
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

class KnightTour {
private:
    int N;
    vector<vector<int>> board;

    // Move pattern for knight (8 possible moves)
    int moveX[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    int moveY[8] = {1, 2, 2, 1, -1, -2, -2, -1};

public:
    KnightTour(int boardSize) : N(boardSize) {
        board.resize(N, vector<int>(N, -1));
    }

    bool isValid(int x, int y) {
        return (x >= 0 && y >= 0 && x < N && y < N && board[x][y] == -1);
    }

    // Try to solve Knight's Tour using backtracking
    bool solve() {
        // Start knight from (0, 0)
        board[0][0] = 0;

        if (solveUtil(0, 0, 1)) {
            printSolution();
            return true;
        } else {
            cout << "Solution does not exist!" << endl;
            return false;
        }
    }

    bool solveUtil(int x, int y, int moveCount) {
        // Base case: If all moves are done
        if (moveCount == N * N) {
            return true;
        }

        // Try all possible moves from current position
        for (int i = 0; i < 8; i++) {
            int nextX = x + moveX[i];
            int nextY = y + moveY[i];

            if (isValid(nextX, nextY)) {
                // Make move
                board[nextX][nextY] = moveCount;

                // Recur to solve rest of the tour
                if (solveUtil(nextX, nextY, moveCount + 1)) {
                    return true;
                }

                // Backtrack
                board[nextX][nextY] = -1;
            }
        }

        return false;
    }

    // Warnsdorff's algorithm for faster solution
    bool solveWarnsdorff() {
        // Start knight from (0, 0)
        board[0][0] = 0;

        if (solveWarnsdorffUtil(0, 0, 1)) {
            printSolution();
            return true;
        } else {
            cout << "Solution does not exist!" << endl;
            return false;
        }
    }

    bool solveWarnsdorffUtil(int x, int y, int moveCount) {
        // Base case: If all moves are done
        if (moveCount == N * N) {
            return true;
        }

        // Order next moves based on accessibility
        vector<pair<int, pair<int, int>>> nextMoves;

        for (int i = 0; i < 8; i++) {
            int nextX = x + moveX[i];
            int nextY = y + moveY[i];

            if (isValid(nextX, nextY)) {
                // Count how many unvisited neighbors this move has
                int accessCount = 0;
                for (int j = 0; j < 8; j++) {
                    int neighborX = nextX + moveX[j];
                    int neighborY = nextY + moveY[j];
                    if (isValid(neighborX, neighborY)) {
                        accessCount++;
                    }
                }

                // Store move and its accessibility score
                nextMoves.push_back({accessCount, {nextX, nextY}});
            }
        }

        // Sort moves by accessibility (ascending)
        sort(nextMoves.begin(), nextMoves.end());

        // Try moves in order of accessibility
        for (auto& move : nextMoves) {
            int nextX = move.second.first;
            int nextY = move.second.second;

            // Make move
            board[nextX][nextY] = moveCount;

            // Recur
            if (solveWarnsdorffUtil(nextX, nextY, moveCount + 1)) {
                return true;
            }

            // Backtrack
            board[nextX][nextY] = -1;
        }

        return false;
    }

    void printSolution() {
        cout << "Knight's Tour Solution:" << endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << setw(3) << board[i][j] << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    // 5x5 board
    KnightTour kt1(5);
    cout << "Solving Knight's Tour on 5x5 board using Warnsdorff's algorithm..." << endl;
    kt1.solveWarnsdorff();

    // 8x8 board (standard chess board)
    KnightTour kt2(8);
    cout << "\nSolving Knight's Tour on 8x8 board using Warnsdorff's algorithm..." << endl;
    kt2.solveWarnsdorff();

    return 0;
}
