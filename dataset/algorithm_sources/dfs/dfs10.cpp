#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int N = 100;
vector<int> adj[N];
bool visited[N];
int dp[N]; // dp[node] stores the longest path from node

int dfs(int node) {
    if (visited[node]) return dp[node];

    visited[node] = true;
    int maxLength = 0;

    for (int neighbor : adj[node]) {
        maxLength = max(maxLength, dfs(neighbor));
    }

    dp[node] = maxLength + 1;
    return dp[node];
}

int main() {
    int n = 6;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {1, 5}};
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
    }

    int longestPath = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            longestPath = max(longestPath, dfs(i));
        }
    }

    cout << "Longest Path Length: " << longestPath << endl;
    return 0;
}

