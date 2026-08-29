// Example 5: DFS for finding all possible words in a Boggle game
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
using namespace std;

class Boggle {
private:
    vector<vector<char>> board;
    int M, N;
    unordered_set<string> dictionary; // Words to find
    unordered_set<string> foundWords; // Words found

    // 8 neighbors movement
    int rowNum[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int colNum[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

public:
    Boggle(vector<vector<char>>& grid, vector<string>& dict) : board(grid) {
        M = board.size();
        N = board[0].size();

        // Add all valid words to the dictionary
        for (string& word : dict) {
            dictionary.insert(word);
        }
    }

    void findWords() {
        // Keep track of visited cells
        vector<vector<bool>> visited(M, vector<bool>(N, false));
        string str = "";

        // Consider each character as starting point and search all words
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                DFS(i, j, visited, str);
            }
        }

        // Print all words found
        cout << "Words found in the Boggle board:" << endl;
        for (const string& word : foundWords) {
            cout << word << endl;
        }
        cout << "Total words found: " << foundWords.size() << endl;
    }

    void DFS(int row, int col, vector<vector<bool>>& visited, string& str) {
        // Mark current cell as visited and append the character
        visited[row][col] = true;
        str += board[row][col];

        // If word is in dictionary, add to found words
        if (dictionary.find(str) != dictionary.end()) {
            foundWords.insert(str);
        }

        // Traverse all adjacent cells
        for (int k = 0; k < 8; k++) {
            int newRow = row + rowNum[k];
            int newCol = col + colNum[k];

            // Check if valid cell and not visited
            if (isValid(newRow, newCol) && !visited[newRow][newCol]) {
                DFS(newRow, newCol, visited, str);
            }
        }

        // Backtrack: remove character and mark cell as not visited
        str.pop_back();
        visited[row][col] = false;
    }

    bool isValid(int row, int col) {
        return (row >= 0 && row < M && col >= 0 && col < N);
    }
};

int main() {
    // Sample Boggle board
    vector<vector<char>> board = {
        {'G', 'I', 'Z'},
        {'U', 'E', 'K'},
        {'Q', 'S', 'E'}
    };

    // Dictionary of valid words
    vector<string> dictionary = {
        "GEEKS", "QUIZ", "GO", "SEE", "USE", "GUQ", "EEK"
    };

    Boggle boggle(board, dictionary);
    boggle.findWords();

    // Larger Boggle board
    vector<vector<char>> board2 = {
        {'T', 'H', 'I', 'S'},
        {'W', 'A', 'T', 'S'},
        {'O', 'A', 'H', 'G'},
        {'F', 'G', 'D', 'T'}
    };

    // Dictionary for the larger board
    vector<string> dictionary2 = {
        "THIS", "THAT", "WATTS", "FAT", "HAT", "TAG", "WAS", "TAT", "HOG", "HOW"
    };

    cout << "\nLarger Boggle board:" << endl;
    Boggle boggle2(board2, dictionary2);
    boggle2.findWords();

    return 0;
}
