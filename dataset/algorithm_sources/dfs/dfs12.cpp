#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];

void dfs(int node, int destination, vector<int>& path) {
    visited[node] = true;
    path.push_back(node);

    if (node == destination) {
        for (int v : path)
            cout << v << " ";
        cout << endl;
    }
    else {
        for (int neighbor : adj[node]) {
            if (!visited[neighbor])
                dfs(neighbor, destination, path);
        }
    }

    path.pop_back();
    visited[node] = false;
}

int main() {
    int n = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {0, 2}, {2, 3}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
    }

    vector<int> path;
    cout << "Paths from 0 to 3:" << endl;
    dfs(0, 3, path);

    return 0;
}

