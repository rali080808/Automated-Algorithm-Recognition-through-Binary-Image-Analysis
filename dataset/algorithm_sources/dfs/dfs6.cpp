#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
int low[N], disc[N], parent[N];
bool visited[N];
int timeCounter = 0;

void dfs(int node) {
    visited[node] = true;
    disc[node] = low[node] = ++timeCounter;

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            parent[neighbor] = node;
            dfs(neighbor);

            low[node] = min(low[node], low[neighbor]);

            if (low[neighbor] > disc[node]) {
                cout << "Bridge: " << node << " - " << neighbor << endl;
            }
        }
        else if (neighbor != parent[node]) {
            low[node] = min(low[node], disc[neighbor]);
        }
    }
}

int main() {
    int n = 5, m = 5;
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 2}, {1, 3}, {3, 4}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    for (int i = 0; i < n; i++) {
        if (!visited[i])
            dfs(i);
    }

    return 0;
}

