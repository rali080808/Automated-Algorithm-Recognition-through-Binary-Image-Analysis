// Example 10: DFS tree and back edge classification
#include <iostream>
#include <vector>
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
    }

    void DFSUtil(int u, vector<bool>& visited, vector<int>& discovery, vector<int>& finish,
                vector<int>& parent, vector<pair<int, int>>& treeEdges,
                vector<pair<int, int>>& backEdges) {
        // Mark the current node as visited
        visited[u] = true;

        // Initialize discovery time
        discovery[u] = ++time;

        // Recur for all adjacent vertices
        for (int v : adj[u]) {
            // If v is not visited
            if (!visited[v]) {
                parent[v] = u;
                treeEdges.push_back({u, v}); // Tree edge
                DFSUtil(v, visited, discovery, finish, parent, treeEdges, backEdges);
            }
            // If v is visited and not parent of u
            else if (discovery[v] < discovery[u] && v != parent[u]) {
                backEdges.push_back({u, v}); // Back edge
            }
        }

        // Set finish time
        finish[u] = ++time;
    }

    void classifyEdges() {
        vector<bool> visited(V, false);
        vector<int> discovery(V, 0);
        vector<int> finish(V, 0);
        vector<int> parent(V, -1);
        vector<pair<int, int>> treeEdges;
        vector<pair<int, int>> backEdges;

        // Call DFS
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                DFSUtil(i, visited, discovery, finish, parent, treeEdges, backEdges);
            }
        }

        // Print edge classification
        cout << "Tree Edges (part of DFS tree):" << endl;
        for (auto& edge : treeEdges) {
            cout << edge.first << " -> " << edge.second << endl;
        }

        cout << "\nBack Edges (connect to ancestors in DFS tree):" << endl;
        for (auto& edge : backEdges) {
            cout << edge.first << " -> " << edge.second << endl;
        }

        // Print discovery and finish times
        cout << "\nVertex Discovery/Finish Times:" << endl;
        for (int i = 0; i < V; i++) {
            cout << "Vertex " << i << ": " << discovery[i] << "/" << finish[i] << endl;
        }
    }
};

int main() {
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(4, 0); // Back edge
    g.addEdge(5, 3); // Cross edge

    cout << "DFS Edge Classification:" << endl;
    g.classifyEdges();

    return 0;
}
