// Example 9: DFS to find strongly connected components (Kosaraju's algorithm)
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;
    vector<vector<int>> transpose;

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(V);
        transpose.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        transpose[v].push_back(u); // Store transpose for second DFS
    }

    void fillOrder(int v, vector<bool>& visited, stack<int>& Stack) {
        // Mark the current node as visited
        visited[v] = true;

        // Recur for all adjacent vertices
        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                fillOrder(neighbor, visited, Stack);
            }
        }

        // Push current vertex to stack which stores vertices by finishing time
        Stack.push(v);
    }

    void DFSUtil(int v, vector<bool>& visited, vector<int>& component) {
        // Mark the current node as visited and print it
        visited[v] = true;
        component.push_back(v);

        // Recur for all adjacent vertices
        for (int neighbor : transpose[v]) {
            if (!visited[neighbor]) {
                DFSUtil(neighbor, visited, component);
            }
        }
    }

    vector<vector<int>> findSCCs() {
        vector<vector<int>> sccs;
        stack<int> Stack;

        // Mark all vertices as not visited (for first DFS)
        vector<bool> visited(V, false);

        // Fill vertices in stack according to their finishing times
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                fillOrder(i, visited, Stack);
            }
        }

        // Mark all vertices as not visited (for second DFS)
        fill(visited.begin(), visited.end(), false);

        // Process all vertices in order defined by Stack
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            // Print Strongly Connected Component of popped vertex
            if (!visited[v]) {
                vector<int> component;
                DFSUtil(v, visited, component);
                sccs.push_back(component);
            }
        }

        return sccs;
    }
};

int main() {
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    g.addEdge(1, 3);
    g.addEdge(3, 4);

    vector<vector<int>> sccs = g.findSCCs();

    cout << "Strongly Connected Components:" << endl;
    for (int i = 0; i < sccs.size(); i++) {
        cout << "Component " << i+1 << ": ";
        for (int v : sccs[i]) {
            cout << v << " ";
        }
        cout << endl;
    }

    return 0;
}
