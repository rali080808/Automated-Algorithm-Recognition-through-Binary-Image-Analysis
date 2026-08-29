// Example 7: DFS for articulation points (cut vertices)
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

    void DFSUtil(int u, vector<bool>& visited, vector<int>& disc,
                vector<int>& low, vector<int>& parent, vector<bool>& isArticulation) {
        // Count of children in DFS Tree
        int children = 0;

        // Mark current node as visited
        visited[u] = true;

        // Initialize discovery time and low value
        disc[u] = low[u] = ++time;

        // Go through all neighbors
        for (int v : adj[u]) {
            // If v is not visited
            if (!visited[v]) {
                children++;
                parent[v] = u;

                DFSUtil(v, visited, disc, low, parent, isArticulation);

                // Check if subtree rooted with v has a connection to an ancestor of u
                low[u] = min(low[u], low[v]);

                // u is an articulation point in following cases

                // (1) u is root of DFS tree and has two or more children
                if (parent[u] == -1 && children > 1) {
                    isArticulation[u] = true;
                }

                // (2) If u is not root and low value of one of its children is more than discovery value of u
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                }
            }
            // Update low value of u for parent function calls
            else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }

    vector<int> findArticulationPoints() {
        vector<bool> visited(V, false);
        vector<int> disc(V, 0);
        vector<int> low(V, 0);
        vector<int> parent(V, -1);
        vector<bool> isArticulation(V, false);

        // Call the recursive helper function for all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                DFSUtil(i, visited, disc, low, parent, isArticulation);
            }
        }

        // Collect articulation points
        vector<int> articulationPoints;
        for (int i = 0; i < V; i++) {
            if (isArticulation[i]) {
                articulationPoints.push_back(i);
            }
        }

        return articulationPoints;
    }
};

int main() {
    Graph g(5);
    g.addEdge(1, 0);
    g.addEdge(0, 2);
    g.addEdge(2, 1);
    g.addEdge(0, 3);
    g.addEdge(3, 4);

    vector<int> artPoints = g.findArticulationPoints();

    cout << "Articulation Points (Vertices whose removal disconnects the graph):" << endl;
    for (int vertex : artPoints) {
        cout << vertex << " ";
    }
    cout << endl;

    return 0;
}
