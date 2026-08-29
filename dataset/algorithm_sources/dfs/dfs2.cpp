#include <iostream>
#include <vector>
#include <stack>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];
stack<int> order;

void dfs(int node) {
    visited[node] = true;
    for (int neighbor : adj[node])
        if (!visited[neighbor])
            dfs(neighbor);
    order.push(node);
}

int main() {
    int n = 6;
    vector<pair<int, int>> edges = {{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}};
    for (auto [u, v] : edges)
        adj[u].push_back(v);

    for (int i = 0; i < n; i++)
        if (!visited[i])
            dfs(i);

    cout << "Topological Sort: ";
    while (!order.empty()) {
        cout << order.top() << " ";
        order.pop();
    }
    cout << endl;
    return 0;
}

