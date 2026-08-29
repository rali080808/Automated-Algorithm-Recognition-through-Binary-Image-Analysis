#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
int color[N];  // 0: not colored, 1: color 1, -1: color -1

bool dfs(int node, int c) {
    color[node] = c;
    for (int neighbor : adj[node]) {
        if (color[neighbor] == 0) {
            if (!dfs(neighbor, -c))
                return false;
        } else if (color[neighbor] == color[node]) {
            return false; // Same color means it's not bipartite
        }
    }
    return true;
}

int main() {
    int n = 4, m = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    bool bipartite = true;
    for (int i = 0; i < n; i++) {
        if (color[i] == 0 && !dfs(i, 1)) {
            bipartite = false;
            break;
        }
    }

    cout << (bipartite ? "Graph is Bipartite." : "Graph is not Bipartite.") << endl;
    return 0;
}

