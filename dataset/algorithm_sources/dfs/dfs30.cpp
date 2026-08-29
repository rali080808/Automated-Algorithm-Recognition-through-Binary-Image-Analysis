// Example 6: DFS to check if a graph is bipartite
#include <iostream>
#include <vector>
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
        adj[v].push_back(u);
    }

    bool isBipartiteUtil(int src, vector<int>& color) {
        color[src] = 1; // Color the source as 1

        // Create a queue for BFS and enqueue source vertex
        vector<int> queue;
        queue.push_back(src);

        while (!queue.empty()) {
            int u = queue[0];
            queue.erase(queue.begin());

            // Return false if there is a self-loop
            for (int v : adj[u]) {
                if (v == u) return false;

                // If adjacent vertex has not been colored
                if (color[v] == -1) {
                    // Assign alternate color to this adjacent vertex
                    color[v] = 1 - color[u];
                    queue.push_back(v);
                }
                // If adjacent vertex has same color as u
                else if (color[v] == color[u]) {
                    return false;
                }
            }
        }

        return true;
    }

    bool isBipartite() {
        vector<int> color(V, -1); // No color initially

        // Do BFS for disconnected graph
        for (int i = 0; i < V; i++) {
            if (color[i] == -1) {
                if (!isBipartiteUtil(i, color)) {
                    return false;
                }
            }
        }

        return true;
    }

    void printColoring(vector<int>& color) {
        cout << "Vertex coloring:" << endl;
        for (int i = 0; i < V; i++) {
            cout << "Vertex " << i << ": " << (color[i] == 1 ? "Red" : "Blue") << endl;
        }
    }
};

int main() {
    // Bipartite graph example
    Graph g1(4);
    g1.addEdge(0, 1);
    g1.addEdge(1, 2);
    g1.addEdge(2, 3);
    g1.addEdge(3, 0);

    cout << "Graph 1: ";
    if (g1.isBipartite()) {
        cout << "Is Bipartite (2-colorable)" << endl;
        vector<int> color(4, -1);
        g1.isBipartiteUtil(0, color);
        g1.printColoring(color);
    } else {
        cout << "Not Bipartite" << endl;
    }

    // Non-bipartite graph example (odd cycle)
    Graph g2(3);
    g2.addEdge(0, 1);
    g2.addEdge(1, 2);
    g2.addEdge(2, 0);

    cout << "\nGraph 2: ";
    if (g2.isBipartite()) {
        cout << "Is Bipartite (2-colorable)" << endl;
    } else {
        cout << "Not Bipartite (contains odd cycle)" << endl;
    }

    return 0;
}
