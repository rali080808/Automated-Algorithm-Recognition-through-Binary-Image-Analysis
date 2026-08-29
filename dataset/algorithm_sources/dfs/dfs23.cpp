#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
using namespace std;

// Edge structure for weighted graph
struct Edge {
    int from, to;
    int weight;

    Edge(int f, int t, int w) : from(f), to(t), weight(w) {}

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class WeightedGraph {
private:
    int V; // Number of vertices
    vector<vector<pair<int, int>>> adj; // Adjacency list (to, weight)

    // DFS helper for finding MST
    void dfsSpanningTree(int u, vector<bool>& visited,
                        vector<Edge>& spanningTree,
                        vector<unordered_map<int, int>>& edgeWeights) {
        visited[u] = true;

        for (const auto& neighbor : adj[u]) {
            int v = neighbor.first;
            int weight = neighbor.second;

            if (!visited[v]) {
                spanningTree.push_back(Edge(u, v, weight));
                dfsSpanningTree(v, visited, spanningTree, edgeWeights);
            }
        }
    }

    // DFS helper for cycle detection
    bool hasCycleUtil(int u, vector<bool>& visited, vector<bool>& recStack,
                     vector<int>& parent) {
        visited[u] = true;
        recStack[u] = true;

        for (const auto& neighbor : adj[u]) {
            int v = neighbor.first;

            if (!visited[v]) {
                parent[v] = u;
                if (hasCycleUtil(v, visited, recStack, parent))
                    return true;
            }
            else if (recStack[v] && parent[u] != v) {
                return true;
            }
        }

        recStack[u] = false;
        return false;
    }

    // Helper function to check if a set of edges forms a valid tree
    bool isValidTree(const vector<Edge>& edges) {
        // A tree with n vertices must have exactly n-1 edges
        if (edges.size() != V - 1)
            return false;

        // Create an adjacency list from the edges
        vector<vector<int>> treeAdj(V);
        for (const Edge& edge : edges) {
            treeAdj[edge.from].push_back(edge.to);
            treeAdj[edge.to].push_back(edge.from);
        }

        // Check if the graph is connected
        vector<bool> visited(V, false);
        DFSUtil(0, treeAdj, visited);

        // If all vertices are reachable from vertex 0, the tree is connected
        return all_of(visited.begin(), visited.end(), [](bool v) { return v; });
    }

    // DFS utility for connectivity check
    void DFSUtil(int u, const vector<vector<int>>& adj, vector<bool>& visited) {
        visited[u] = true;

        for (int v : adj[u]) {
            if (!visited[v]) {
                DFSUtil(v, adj, visited);
            }
        }
    }

public:
    WeightedGraph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    // Add an undirected weighted edge
    void addEdge(int u, int v, int weight) {
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight});
    }

    // Find a spanning tree using DFS
    vector<Edge> findDFSSpanningTree(int startVertex = 0) {
        vector<bool> visited(V, false);
        vector<Edge> spanningTree;
        vector<unordered_map<int, int>> edgeWeights(V);

        // Precompute edge weights for quick lookup
        for (int u = 0; u < V; u++) {
            for (const auto& neighbor : adj[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;
                edgeWeights[u][v] = weight;
            }
        }

        dfsSpanningTree(startVertex, visited, spanningTree, edgeWeights);

        return spanningTree;
    }

    // Find a Minimum Spanning Tree using DFS and edge sorting
    vector<Edge> findMSTUsingDFS() {
        // Get all edges
        vector<Edge> allEdges;

        for (int u = 0; u < V; u++) {
            for (const auto& neighbor : adj[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;

                // Add edge only once (u < v to avoid duplicates)
                if (u < v) {
                    allEdges.push_back(Edge(u, v, weight));
                }
            }
        }

        // Sort edges by weight
        sort(allEdges.begin(), allEdges.end());

        // Try to construct a valid tree
        vector<Edge> mst;

        for (const Edge& edge : allEdges) {
            mst.push_back(edge);

            // Check if adding this edge creates a valid tree
            if (isValidTree(mst)) {
                continue;
            }

            // If not valid, remove the edge
            mst.pop_back();

            // If we have n-1 edges, we're done
            if (mst.size() == V - 1) {
                break;
            }
        }

        return mst;
    }

    // Check if there's a cycle in the graph
    bool hasCycle() {
        vector<bool> visited(V, false);
        vector<bool> recStack(V, false);
        vector<int> parent(V, -1);

        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                if (hasCycleUtil(i, visited, recStack, parent)) {
                    return true;
                }
            }
        }

        return false;
    }

    // A different approach: find MST using Reverse-Delete algorithm
    // (a variant of Kruskal's that uses DFS for cycle detection)
    vector<Edge> findMSTUsingReverseDelete() {
        // Get all edges in the original graph
        vector<Edge> allEdges;

        for (int u = 0; u < V; u++) {
            for (const auto& neighbor : adj[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;

                // Add edge only once (u < v to avoid duplicates)
                if (u < v) {
                    allEdges.push_back(Edge(u, v, weight));
                }
            }
        }

        // Sort edges by weight in descending order
        sort(allEdges.begin(), allEdges.end(), [](const Edge& a, const Edge& b) {
            return a.weight > b.weight;
        });

        // Create a copy of the graph
        WeightedGraph mstGraph(V);
        for (const Edge& edge : allEdges) {
            mstGraph.addEdge(edge.from, edge.to, edge.weight);
        }

        // Iterate through edges in descending order of weight
        for (const Edge& edge : allEdges) {
            // Remove the edge
            mstGraph.removeEdge(edge.from, edge.to);

            // Check if removing the edge disconnects the graph
            if (!mstGraph.isConnected()) {
                // If disconnected, add the edge back
                mstGraph.addEdge(edge.from, edge.to, edge.weight);
            }
        }

        // Get the final MST edges
        vector<Edge> mstEdges;
        for (int u = 0; u < V; u++) {
            for (const auto& neighbor : mstGraph.getNeighbors(u)) {
                int v = neighbor.first;
                int weight = neighbor.second;

                // Add edge only once (u < v to avoid duplicates)
                if (u < v) {
                    mstEdges.push_back(Edge(u, v, weight));
                }
            }
        }

        return mstEdges;
    }

    // Remove an edge from the graph
    void removeEdge(int u, int v) {
        // Remove v from adjacency list of u
        adj[u].erase(
            remove_if(adj[u].begin(), adj[u].end(),
                     [v](const pair<int, int>& p) { return p.first == v; }),
            adj[u].end()
        );

        // Remove u from adjacency list of v
        adj[v].erase(
            remove_if(adj[v].begin(), adj[v].end(),
                     [u](const pair<int, int>& p) { return p.first == u; }),
            adj[v].end()
        );
    }

    // Check if the graph is connected using DFS
    bool isConnected() {
        vector<bool> visited(V, false);
        DFSUtil(0, adj, visited);

        return all_of(visited.begin(), visited.end(), [](bool v) { return v; });
    }

    // DFS utility for connectivity check using adjacency list
    void DFSUtil(int u, const vector<vector<pair<int, int>>>& adj, vector<bool>& visited) {
        visited[u] = true;

        for (const auto& neighbor : adj[u]) {
            int v = neighbor.first;
            if (!visited[v]) {
                DFSUtil(v, adj, visited);
            }
        }
    }

    // Get neighbors of a vertex
    const vector<pair<int, int>>& getNeighbors(int u) const {
        return adj[u];
    }

    // Calculate the total weight of a spanning tree
    int calculateTreeWeight(const vector<Edge>& tree) {
        int totalWeight = 0;
        for (const Edge& edge : tree) {
            totalWeight += edge.weight;
        }
        return totalWeight;
    }

    // Print a spanning tree
    void printTree(const vector<Edge>& tree, const string& treeName) {
        int totalWeight = calculateTreeWeight(tree);

        cout << treeName << " (" << totalWeight << "):" << endl;
        for (const Edge& edge : tree) {
            cout << edge.from << " -- " << edge.to << " (weight: " << edge.weight << ")" << endl;
        }
        cout << endl;
    }
};

int main() {
    // Example 1: Simple weighted graph
    WeightedGraph g1(6);
    g1.addEdge(0, 1, 4);
    g1.addEdge(0, 2, 3);
    g1.addEdge(1, 2, 1);
    g1.addEdge(1, 3, 2);
    g1.addEdge(2, 3, 4);
    g1.addEdge(3, 4, 2);
    g1.addEdge(4, 5, 6);

    // Find a DFS spanning tree
    vector<Edge> dfsTree = g1.findDFSSpanningTree();
    g1.printTree(dfsTree, "DFS Spanning Tree");

    // Find a Minimum Spanning Tree using DFS
    vector<Edge> mstUsingDFS = g1.findMSTUsingDFS();
    g1.printTree(mstUsingDFS, "MST Using DFS");

    // Find a Minimum Spanning Tree using Reverse-Delete
    vector<Edge> mstUsingReverseDelete = g1.findMSTUsingReverseDelete();
    g1.printTree(mstUsingReverseDelete, "MST Using Reverse-Delete");

    // Example 2: Another weighted graph
    WeightedGraph g2(7);
    g2.addEdge(0, 1, 7);
    g2.addEdge(0, 3, 5);
    g2.addEdge(1, 2, 8);
    g2.addEdge(1, 3, 9);
    g2.addEdge(1, 4, 7);
    g2.addEdge(2, 4, 5);
    g2.addEdge(3, 4, 15);
    g2.addEdge(3, 5, 6);
    g2.addEdge(4, 5, 8);
    g2.addEdge(4, 6, 9);
    g2.addEdge(5, 6, 11);

    // Find a DFS spanning tree
    vector<Edge> dfsTree2 = g2.findDFSSpanningTree();
    g2.printTree(dfsTree2, "DFS Spanning Tree");

    // Find a Minimum Spanning Tree using DFS
    vector<Edge> mstUsingDFS2 = g2.findMSTUsingDFS();
    g2.printTree(mstUsingDFS2, "MST Using DFS");

    // Find a Minimum Spanning Tree using Reverse-Delete
    vector<Edge> mstUsingReverseDelete2 = g2.findMSTUsingReverseDelete();
    g2.printTree(mstUsingReverseDelete2, "MST Using Reverse-Delete");

    return 0;
}
