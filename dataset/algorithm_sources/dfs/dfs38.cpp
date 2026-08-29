// Example 3: DFS for finding eulerian path/circuit in a graph
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
        adj[v].push_back(u); // For undirected graph
    }

    // Check if all non-zero degree vertices are connected
    bool isConnected() {
        vector<bool> visited(V, false);

        // Find a vertex with non-zero degree
        int i;
        for (i = 0; i < V; i++) {
            if (adj[i].size() > 0) {
                break;
            }
        }

        // If no vertices with non-zero degree, graph is connected
        if (i == V) {
            return true;
        }

        // Start DFS from vertex with non-zero degree
        DFS(i, visited);

        // Check if all non-zero degree vertices are visited
        for (i = 0; i < V; i++) {
            if (!visited[i] && adj[i].size() > 0) {
                return false;
            }
        }

        return true;
    }

    void DFS(int v, vector<bool>& visited) {
        visited[v] = true;

        for (int u : adj[v]) {
            if (!visited[u]) {
                DFS(u, visited);
            }
        }
    }

    // Count vertices with odd degree
    int countOddDegreeVertices() {
        int count = 0;
        for (int i = 0; i < V; i++) {
            if (adj[i].size() & 1) { // Check if odd
                count++;
            }
        }
        return count;
    }

    // Find Eulerian path or circuit
    void findEulerianPath() {
        // Check if graph is connected
        if (!isConnected()) {
            cout << "Graph is not connected, no Eulerian path exists." << endl;
            return;
        }

        // Count odd degree vertices
        int oddDegreeCount = countOddDegreeVertices();

        if (oddDegreeCount > 2) {
            cout << "No Eulerian path or circuit exists." << endl;
            return;
        }

        // Find a starting vertex
        int startVertex = 0;

        if (oddDegreeCount == 2) {
            cout << "Eulerian path exists (but not a circuit)." << endl;
            // Start from a vertex with odd degree
            for (int i = 0; i < V; i++) {
                if (adj[i].size() & 1) {
                    startVertex = i;
                    break;
                }
            }
        } else {
            cout << "Eulerian circuit exists." << endl;
            // Start from any vertex with non-zero degree
            for (int i = 0; i < V; i++) {
                if (adj[i].size() > 0) {
                    startVertex = i;
                    break;
                }
            }
        }

        // Print the Eulerian path or circuit
        printEulerianPath(startVertex);
    }

    void printEulerianPath(int startVertex) {
        // Create a copy of the adjacency list
        vector<vector<int>> adjCopy = adj;

        stack<int> currentPath;
        vector<int> circuit;

        currentPath.push(startVertex);
        int currentVertex = startVertex;

        while (!currentPath.empty()) {
            // If current vertex has neighbors
            if (!adjCopy[currentVertex].empty()) {
                // Push current vertex to stack
                currentPath.push(currentVertex);

                // Get next neighbor
                int nextVertex = adjCopy[currentVertex].back();
                adjCopy[currentVertex].pop_back();

                // Remove the edge in both directions
                for (auto it = adjCopy[nextVertex].begin(); it != adjCopy[nextVertex].end(); ++it) {
                    if (*it == currentVertex) {
                        adjCopy[nextVertex].erase(it);
                        break;
                    }
                }

                // Move to next vertex
                currentVertex = nextVertex;
            } else {
                // Add current vertex to circuit
                circuit.push_back(currentVertex);

                // Backtrack to explore more paths
                currentVertex = currentPath.top();
                currentPath.pop();
            }
        }

        // Print the Eulerian path
        cout << "Eulerian path: ";
        for (int i = circuit.size() - 1; i >= 0; i--) {
            cout << circuit[i];
            if (i > 0) cout << " -> ";
        }
        cout << endl;
    }
};

int main() {
    // Example 1: Graph with Eulerian circuit
    Graph g1(5);
    g1.addEdge(0, 1);
    g1.addEdge(0, 2);
    g1.addEdge(1, 2);
    g1.addEdge(0, 3);
    g1.addEdge(3, 4);
    g1.addEdge(0, 4);

    cout << "Graph 1:" << endl;
    g1.findEulerianPath();

    // Example 2: Graph with Eulerian path (not circuit)
    Graph g2(5);
    g2.addEdge(0, 1);
    g2.addEdge(0, 2);
    g2.addEdge(1, 2);
    g2.addEdge(0, 3);
    g2.addEdge(3, 4);

    cout << "\nGraph 2:" << endl;
    g2.findEulerianPath();

    // Example 3: Graph with no Eulerian path or circuit
    Graph g3(5);
    g3.addEdge(0, 1);
    g3.addEdge(1, 2);
    g3.addEdge(2, 3);
    g3.addEdge(3, 4);

    cout << "\nGraph 3:" << endl;
    g3.findEulerianPath();

    return 0;
}
