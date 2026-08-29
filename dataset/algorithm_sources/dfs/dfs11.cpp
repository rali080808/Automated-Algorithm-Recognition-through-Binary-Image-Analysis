#include <iostream>
#include <vector>
#include <stack>
using namespace std;

const int N = 100;
vector<int> adj[N], rev_adj[N];
bool visited[N];
stack<int> order;

void dfs1(int node) {
    visited[node] = true;
    for (int neighbor : adj[node]) {
        if (!visited[neighbor])
            dfs1(neighbor);
    }
    order.push(node);
}

void dfs2(int node) {
    visited[node] = true;
    cout << node << " ";
    for (int neighbor : rev_adj[node]) {
        if (!visited[neighbor])
            dfs2(neighbor);
    }
}

int main() {
    int n = 5, m = 5;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}, {3, 4}};

    // Build graph and reverse graph
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        rev_adj[v].push_back(u);
    }

    // Step 1: Fill order stack
    for (int i = 0; i < n; i++) {
        if (!visited[i])
            dfs1(i);
    }

    // Step 2: Reverse DFS based on stack
    fill(begin(visited), end(visited), false);
    cout << "Strongly Connected Components:" << endl;
    while (!order.empty()) {
        int node = order.top();
        order.pop();
        if (!visited[node]) {
            dfs2(node);
            cout << endl;
        }
    }

    return 0;
}

