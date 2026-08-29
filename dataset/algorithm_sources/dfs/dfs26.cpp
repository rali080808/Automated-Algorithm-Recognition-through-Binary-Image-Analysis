#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

vector<pair<int, int>> directions = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
};

bool isValid(int x, int y, int n, int m, vector<vector<bool>>& visited) {
    return (x >= 0 && x < n && y >= 0 && y < m && !visited[x][y]);
}

void dfs(vector<vector<char>>& board, vector<vector<bool>>& visited,
         int x, int y, string current, unordered_set<string>& dictionary, unordered_set<string>& found) {

    current += board[x][y];

    if (dictionary.count(current)) {
        found.insert(current);
    }

    visited[x][y] = true;

    for (auto [dx, dy] : directions) {
        int nx = x + dx;
        int ny = y + dy;
        if (isValid(nx, ny, board.size(), board[0].size(), visited)) {
            dfs(board, visited, nx, ny, current, dictionary, found);
        }
    }

    visited[x][y] = false;
}

int main() {
    vector<vector<char>> board = {
        {'t', 'a', 'p'},
        {'n', 'o', 'e'},
        {'s', 'i', 'r'}
    };

    unordered_set<string> dictionary = {"tap", "tone", "tire", "top", "not", "note", "piano", "rise"};
    unordered_set<string> found;

    int n = board.size();
    int m = board[0].size();
    vector<vector<bool>> visited(n, vector<bool>(m, false));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            dfs(board, visited, i, j, "", dictionary, found);
        }
    }

    cout << "Found words:\n";
    for (const string& word : found) {
        cout << word << "\n";
    }

    return 0;
}

