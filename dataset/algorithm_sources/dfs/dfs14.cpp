#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
int low[N], disc[N], parent[N];
bool visited[N];
bool articulationPoint[N];
int timeCounter = 0;

void dfs(int node) {
    visited[node] = true;
    disc[node] = low[node] = ++timeCounter;
    int children = 0;

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            parent[neighbor] = node;
            children++;
            dfs(neighbor);

            low[node] = min(low[node], low[neighbor]);

            if (parent[node] == -1 && children > 1)
                articulationPoint[node] = true;
            else if (parent[node] != -1 && low[neighbor] >= disc[node])
                articulationPoint[node] = true;
        } else if (neighbor != parent[node]) {
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

    cout << "Articulation Points: ";
    for (int i = 0; i < n; i++) {
        if (articulationPoint[i])
            cout << i << " ";
    }
    cout << endl;

    return 0;
}

