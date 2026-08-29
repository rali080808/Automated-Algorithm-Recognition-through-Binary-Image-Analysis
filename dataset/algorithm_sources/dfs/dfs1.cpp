#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];

void dfs(int node) {
    visited[node] = true;
    for (int neighbor : adj[node])
        if (!visited[neighbor])
            dfs(neighbor);
}

int main() {
    int n = 6, m = 4;
    vector<pair<int, int>> edges = {{1, 2}, {2, 3}, {4, 5}, {5, 6}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int components = 0;
    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            dfs(i);
            components++;
        }
    }

    cout << "Number of connected components: " << components << endl;
    return 0;
}

