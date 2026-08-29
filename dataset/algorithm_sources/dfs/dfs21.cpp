#include <iostream>
#include <vector>
#include <stack>
using namespace std;

class Graph {
private:
    int V; // Number of vertices
    vector<vector<int>> adj; // Adjacency list

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    // Add an edge to the graph
    void addEdge(int v, int w) {
        adj[v].push_back(w);
    }

    // Iterative DFS using a stack
    void DFSIterative(int startVertex) {
        // Mark all vertices as not visited
        vector<bool> visited(V, false);

        // Create a stack for DFS
        stack<int> dfsStack;

        // Push the starting vertex
        dfsStack.push(startVertex);

        while (!dfsStack.empty()) {
            // Pop a vertex from stack
            int v = dfsStack.top();
            dfsStack.pop();

            // Skip if already visited
            if (visited[v])
                continue;

            // Mark the current node as visited and print it
            visited[v] = true;
            cout << v << " ";

            // Push all adjacent vertices that are not visited
            // Note: Push in reverse order to match recursive DFS order
            for (int i = adj[v].size() - 1; i >= 0; i--) {
                int neighbor = adj[v][i];
                if (!visited[neighbor]) {
                    dfsStack.push(neighbor);
                }
            }
        }
    }

    // Complete iterative DFS for disconnected graph
    void DFSIterativeComplete() {
        // Mark all vertices as not visited
        vector<bool> visited(V, false);

        // Process all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                // Use an inner DFS with the same visited array
                stack<int> dfsStack;
                dfsStack.push(i);

                while (!dfsStack.empty()) {
                    int v = dfsStack.top();
                    dfsStack.pop();

                    if (visited[v])
                        continue;

                    visited[v] = true;
                    cout << v << " ";

                    for (int j = adj[v].size() - 1; j >= 0; j--) {
                        int neighbor = adj[v][j];
                        if (!visited[neighbor]) {
                            dfsStack.push(neighbor);
                        }
                    }
                }
            }
        }
    }
};

int main() {
    // Create a graph
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 5);
    g.addEdge(4, 5);

    cout << "Iterative Depth First Traversal starting from vertex 0: ";
    g.DFSIterative(0);
    cout << endl;

    // Create a disconnected graph
    Graph g2(8);
    g2.addEdge(0, 1);
    g2.addEdge(0, 2);
    g2.addEdge(1, 2);
    g2.addEdge(3, 4);
    g2.addEdge(5, 6);
    g2.addEdge(5, 7);
    g2.addEdge(6, 7);

    cout << "Iterative Depth First Traversal for disconnected graph: ";
    g2.DFSIterativeComplete();
    cout << endl;

    return 0;
}
