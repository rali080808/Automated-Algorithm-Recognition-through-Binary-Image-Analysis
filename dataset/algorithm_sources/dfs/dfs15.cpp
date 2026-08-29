#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];
vector<int> path;

bool dfs(int node, int n) {
    path.push_back(node);
    visited[node] = true;

    if (path.size() == n) {
        return true;
    }

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            if (dfs(neighbor, n)) {
                return true;
            }
        }
    }

    visited[node] = false;
    path.pop_back();
    return false;
}

int main() {
    int n = 5;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    bool found = false;
    for (int i = 0; i < n; i++) {
        fill(begin(visited), end(visited), false);
        path.clear();
        if (dfs(i, n)) {
            found = true;
            break;
        }
    }

    if (found) {
        cout << "Hamiltonian Path: ";
        for (int node : path)
            cout << node << " ";
        cout << endl;
    } else {
        cout << "No Hamiltonian Path exists." << endl;
    }

    return 0;
}

