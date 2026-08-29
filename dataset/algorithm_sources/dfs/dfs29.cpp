// Example 4: DFS for topological sorting
#include <iostream>
#include <vector>
#include <stack>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack) {
        // Mark current node as visited
        visited[v] = true;

        // Recur for all adjacent vertices
        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                topologicalSortUtil(neighbor, visited, Stack);
            }
        }

        // Push current vertex to stack (happens after all dependencies processed)
        Stack.push(v);
    }

    vector<int> topologicalSort() {
        vector<bool> visited(V, false);
        stack<int> Stack;

        // Call recursive helper for all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, Stack);
            }
        }

        // Create result array from stack
        vector<int> result;
        while (!Stack.empty()) {
            result.push_back(Stack.top());
            Stack.pop();
        }

        return result;
    }
};

int main() {
    // Graph representing tasks with dependencies
    Graph g(6);
    g.addEdge(5, 2);
    g.addEdge(5, 0);
    g.addEdge(4, 0);
    g.addEdge(4, 1);
    g.addEdge(2, 3);
    g.addEdge(3, 1);

    // Get tasks in topological order
    vector<int> order = g.topologicalSort();

    cout << "Topological Sorting (tasks in order): ";
    for (int vertex : order) {
        cout << vertex << " ";
    }
    cout << endl;

    return 0;
}
