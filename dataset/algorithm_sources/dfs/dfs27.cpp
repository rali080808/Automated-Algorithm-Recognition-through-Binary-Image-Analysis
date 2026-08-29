// Example 2: DFS to detect cycles in a directed graph
#include <iostream>
#include <vector>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;
    vector<bool> visited;
    vector<bool> recStack; // To track nodes in current recursion stack

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
        visited.resize(V, false);
        recStack.resize(V, false);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    bool isCyclicUtil(int v) {
        // Mark current node as visited and add to recursion stack
        visited[v] = true;
        recStack[v] = true;

        // Check all neighbors
        for (int neighbor : adj[v]) {
            // If not visited, recursively check
            if (!visited[neighbor] && isCyclicUtil(neighbor)) {
                return true;
            }
            // If already in recursion stack, cycle found
            else if (recStack[neighbor]) {
                cout << "Cycle detected: " << v << " -> " << neighbor << endl;
                return true;
            }
        }

        // Remove from recursion stack
        recStack[v] = false;
        return false;
    }

    bool isCyclic() {
        // Call recursive helper for all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i] && isCyclicUtil(i)) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    // Create a graph with a cycle
    Graph g1(4);
    g1.addEdge(0, 1);
    g1.addEdge(1, 2);
    g1.addEdge(2, 3);
    g1.addEdge(3, 1); // Creates a cycle

    cout << "Graph 1: ";
    if (g1.isCyclic()) {
        cout << "Contains cycle" << endl;
    } else {
        cout << "No cycle detected" << endl;
    }

    // Create a graph without cycle
    Graph g2(4);
    g2.addEdge(0, 1);
    g2.addEdge(1, 2);
    g2.addEdge(2, 3);

    cout << "Graph 2: ";
    if (g2.isCyclic()) {
        cout << "Contains cycle" << endl;
    } else {
        cout << "No cycle detected" << endl;
    }

    return 0;
}
