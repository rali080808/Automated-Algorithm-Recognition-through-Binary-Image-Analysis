// Example 1: DFS for finding the longest path in a directed acyclic graph (DAG)
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<pair<int, int>>> adj; // {vertex, weight}

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    void addEdge(int u, int v, int weight) {
        adj[u].push_back({v, weight});
    }

    // Topological Sort using DFS
    void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack) {
        visited[v] = true;

        for (auto& neighbor : adj[v]) {
            if (!visited[neighbor.first]) {
                topologicalSortUtil(neighbor.first, visited, Stack);
            }
        }

        Stack.push(v);
    }

    // Find longest path in DAG
    void longestPath(int src) {
        stack<int> Stack;
        vector<int> dist(V, INT_MIN);
        vector<int> parent(V, -1);
        vector<bool> visited(V, false);

        // Create a topological sort of vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, Stack);
            }
        }

        // Initialize distance to source as 0
        dist[src] = 0;

        // Process vertices in topological order
        while (!Stack.empty()) {
            int u = Stack.top();
            Stack.pop();

            // Update distances of all adjacent vertices
            if (dist[u] != INT_MIN) {
                for (auto& neighbor : adj[u]) {
                    int v = neighbor.first;
                    int weight = neighbor.second;

                    if (dist[v] < dist[u] + weight) {
                        dist[v] = dist[u] + weight;
                        parent[v] = u;
                    }
                }
            }
        }

        // Print longest distances from source
        cout << "Longest distances from source " << src << ":" << endl;
        for (int i = 0; i < V; i++) {
            if (dist[i] == INT_MIN) {
                cout << "Vertex " << i << ": Not reachable" << endl;
            } else {
                cout << "Vertex " << i << ": " << dist[i] << endl;

                // Print path
                cout << "  Path: ";
                vector<int> path;
                for (int at = i; at != -1; at = parent[at]) {
                    path.push_back(at);
                }
                reverse(path.begin(), path.end());

                for (int j = 0; j < path.size(); j++) {
                    cout << path[j];
                    if (j < path.size() - 1) cout << " -> ";
                }
                cout << endl;
            }
        }
    }
};

int main() {
    Graph g(6);
    g.addEdge(0, 1, 5);
    g.addEdge(0, 2, 3);
    g.addEdge(1, 3, 6);
    g.addEdge(1, 2, 2);
    g.addEdge(2, 4, 4);
    g.addEdge(2, 5, 2);
    g.addEdge(2, 3, 7);
    g.addEdge(3, 5, 1);
    g.addEdge(4, 5, -1);

    int source = 0;
    g.longestPath(source);

    return 0;
}
