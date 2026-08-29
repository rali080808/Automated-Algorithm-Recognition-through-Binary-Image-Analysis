// Example 3: DFS to find all paths between two vertices
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
    }

    void findAllPathsUtil(int u, int dest, vector<bool>& visited,
                         vector<int>& path, vector<vector<int>>& allPaths) {
        // Mark current node and add to path
        visited[u] = true;
        path.push_back(u);

        // If current vertex is destination, add path to result
        if (u == dest) {
            allPaths.push_back(path);
        } else {
            // Recur for all adjacent vertices
            for (int neighbor : adj[u]) {
                if (!visited[neighbor]) {
                    findAllPathsUtil(neighbor, dest, visited, path, allPaths);
                }
            }
        }

        // Backtrack: remove current vertex and mark as unvisited
        path.pop_back();
        visited[u] = false;
    }

    vector<vector<int>> findAllPaths(int src, int dest) {
        vector<bool> visited(V, false);
        vector<int> path;
        vector<vector<int>> allPaths;

        findAllPathsUtil(src, dest, visited, path, allPaths);

        return allPaths;
    }
};

int main() {
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 4);
    g.addEdge(3, 4);

    int src = 0, dest = 4;

    vector<vector<int>> allPaths = g.findAllPaths(src, dest);

    cout << "All paths from " << src << " to " << dest << ":" << endl;
    for (const auto& path : allPaths) {
        for (int vertex : path) {
            cout << vertex << " ";
        }
        cout << endl;
    }

    return 0;
}
