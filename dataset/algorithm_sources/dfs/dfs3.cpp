#include <iostream>
#include <vector>
using namespace std;

const int N = 5;
char maze[N][N] = {
    {'S', '.', '#', '.', '.'},
    {'#', '.', '#', '.', '#'},
    {'.', '.', '.', '.', '.'},
    {'#', '#', '#', '#', '.'},
    {'.', '.', '.', '.', 'E'}
};
bool visited[N][N];
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};
bool found = false;

void dfs(int x, int y) {
    if (x < 0 || y < 0 || x >= N || y >= N || visited[x][y] || maze[x][y] == '#')
        return;
    if (maze[x][y] == 'E') {
        found = true;
        return;
    }
    visited[x][y] = true;
    for (int d = 0; d < 4; d++)
        dfs(x + dx[d], y + dy[d]);
}

int main() {
    dfs(0, 0);
    cout << (found ? "Path exists!" : "No path found.") << endl;
    return 0;
}

