#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];

int dfs(int node) {
    visited[node] = true;
    int size = 1;

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            size += dfs(neighbor);
        }
    }

    return size;
}

int main() {
    int n = 7, m = 6;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {5, 6}, {2, 3}};

    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    fill(begin(visited), end(visited), false);
    int largestComponent = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            largestComponent = max(largestComponent, dfs(i));
        }
    }

    cout << "Size of the largest connected component: " << largestComponent << endl;
    return 0;
}

