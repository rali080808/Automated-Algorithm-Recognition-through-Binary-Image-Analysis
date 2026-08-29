#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N], inStack[N];

bool dfs(int node) {
    if (inStack[node]) return true;
    if (visited[node]) return false;

    visited[node] = true;
    inStack[node] = true;

    for (int neighbor : adj[node]) {
        if (dfs(neighbor)) {
            return true;
        }
    }

    inStack[node] = false;
    return false;
}

int main() {
    int n = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
    }

    bool hasCycle = false;
    fill(begin(visited), end(visited), false);
    fill(begin(inStack), end(inStack), false);

    for (int i = 0; i < n; i++) {
        if (!visited[i] && dfs(i)) {
            hasCycle = true;
            break;
        }
    }

    cout << (hasCycle ? "Graph has a cycle." : "Graph does not have a cycle.") << endl;
    return 0;
}

