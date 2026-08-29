#include <iostream>
#include <vector>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];

int countPaths(int node, int k, int length) {
    if (length == k) return 1;

    visited[node] = true;
    int paths = 0;

    for (int neighbor : adj[node]) {
        if (!visited[neighbor]) {
            paths += countPaths(neighbor, k, length + 1);
        }
    }

    visited[node] = false;
    return paths;
}

int main() {
    int n = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
    }

    int k = 2;
    cout << "Number of paths of length " << k << ": " << countPaths(0, k, 0) << endl;

    return 0;
}

