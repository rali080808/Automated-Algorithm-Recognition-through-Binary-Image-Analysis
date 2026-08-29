#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
int farthest_node, max_dist;

void dfs(int node, int parent, int dist) {
    if (dist > max_dist) {
        max_dist = dist;
        farthest_node = node;
    }
    for (int neighbor : adj[node]) {
        if (neighbor != parent)
            dfs(neighbor, node, dist + 1);
    }
}

int main() {
    int n = 6;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {1, 3}, {3, 4}, {4, 5}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    max_dist = -1;
    dfs(0, -1, 0);  // First pass to find the farthest node
    int start = farthest_node;
    max_dist = -1;
    dfs(start, -1, 0);  // Second pass from farthest node

    cout << "Tree diameter is: " << max_dist << endl;
    return 0;
}

