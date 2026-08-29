#include <iostream>
#include <vector>
using namespace std;

const int N = 5;
int grid[N][N] = {
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1}
};
bool visited[N][N];
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

void dfs(int x, int y) {
    visited[x][y] = true;
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d], ny = y + dy[d];
        if (nx >= 0 && ny >= 0 && nx < N && ny < N && grid[nx][ny] == 1 && !visited[nx][ny])
            dfs(nx, ny);
    }
}

int main() {
    int islands = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 1 && !visited[i][j]) {
                dfs(i, j);
                islands++;
            }
        }
    }

    cout << "Number of islands: " << islands << endl;
    return 0;
}

