// Example 8: DFS to find bridges in an undirected graph
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;
    int time;

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
        time = 0;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void bridgeUtil(int u, vector<bool>& visited, vector<int>& disc,
                 vector<int>& low, vector<int>& parent, vector<pair<int, int>>& bridges) {
        // Mark the current node as visited
        visited[u] = true;

        // Initialize discovery time and low value
        disc[u] = low[u] = ++time;

        // Go through all neighbors
        for (int v : adj[u]) {
            // If v is not visited yet, then make it a child of u in DFS tree
            if (!visited[v]) {
                parent[v] = u;
                bridgeUtil(v, visited, disc, low, parent, bridges);

                // Check if the subtree rooted with v has a connection to
                // one of the ancestors of u
                low[u] = min(low[u], low[v]);

                // If the lowest vertex reachable from subtree under v is
                // below u in DFS tree, then u-v is a bridge
                if (low[v] > disc[u]) {
                    bridges.push_back({u, v});
                }
            }
            // Update low value of u for parent function calls
            else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }

    vector<pair<int, int>> findBridges() {
        vector<bool> visited(V, false);
        vector<int> disc(V, 0);
        vector<int> low(V, 0);
        vector<int> parent(V, -1);
        vector<pair<int, int>> bridges;

        // Call the recursive helper function for all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                bridgeUtil(i, visited, disc, low, parent, bridges);
            }
        }

        return bridges;
    }
};

int main() {
    Graph g(5);
    g.addEdge(1, 0);
    g.addEdge(0, 2);
    g.addEdge(2, 1);
    g.addEdge(0, 3);
    g.addEdge(3, 4);

    vector<pair<int, int>> bridges = g.findBridges();

    cout << "Bridges (edges whose removal disconnects the graph):" << endl;
    for (auto& bridge : bridges) {
        cout << bridge.first << " -- " << bridge.second << endl;
    }

    return 0;
}
