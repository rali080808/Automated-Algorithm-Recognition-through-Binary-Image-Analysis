#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N], recStack[N];

bool dfs(int node) {
    visited[node] = true;
    recStack[node] = true;
    for (int neighbor : adj[node]) {
        if (!visited[neighbor] && dfs(neighbor))
            return true;
        else if (recStack[neighbor])
            return true;
    }
    recStack[node] = false;
    return false;
}

int main() {
    int n = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 1}};
    for (auto [u, v] : edges)
        adj[u].push_back(v);

    for (int i = 0; i < n; i++) {
        if (!visited[i] && dfs(i)) {
            cout << "Cycle detected!" << endl;
            return 0;
        }
    }

    cout << "No cycle detected." << endl;
    return 0;
}

