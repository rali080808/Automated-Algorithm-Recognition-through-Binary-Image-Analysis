#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];
vector<int> paths[N];

void dfs(int node, vector<int>& path) {
    visited[node] = true;
    path.push_back(node);

    if (adj[node].empty()) {
        paths[node] = path;  // Store the path to this node
    }

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            dfs(neighbor, path);
        }
    }

    path.pop_back();
    visited[node] = false;
}

int main() {
    int n = 5;
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
    }

    vector<int> path;
    fill(begin(visited), end(visited), false);
    dfs(0, path);

    cout << "Paths from 0 to all other nodes:" << endl;
    for (int i = 0; i < n; i++) {
        cout << "Path to " << i << ": ";
        for (int node : paths[i])
            cout << node << " ";
        cout << endl;
    }

    return 0;
}

