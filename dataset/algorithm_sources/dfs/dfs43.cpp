#include <iostream>
#include <vector>
#include <algorithm>

class ArticulationPointsFinder {
private:
    std::vector<std::vector<int>> graph;
    int numVertices;
    int time;

public:
    ArticulationPointsFinder(int vertices) : numVertices(vertices), time(0) {
        graph.resize(vertices);
    }

    void addEdge(int u, int v) {
        graph[u].push_back(v);
        graph[v].push_back(u); // Undirected graph
    }

    std::vector<int> findArticulationPoints() {
        std::vector<int> disc(numVertices, -1); // Discovery times
        std::vector<int> low(numVertices, -1);  // Earliest visited vertex reachable
        std::vector<bool> visited(numVertices, false);
        std::vector<bool> isArticulation(numVertices, false);
        std::vector<int> parent(numVertices, -1);

        // DFS for each unvisited vertex
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                dfsArticulationPoints(i, disc, low, visited, parent, isArticulation);
            }
        }

        // Collect results
        std::vector<int> result;
        for (int i = 0; i < numVertices; i++) {
            if (isArticulation[i]) {
                result.push_back(i);
            }
        }

        return result;
    }

private:
    void dfsArticulationPoints(int u, std::vector<int>& disc, std::vector<int>& low,
                               std::vector<bool>& visited, std::vector<int>& parent,
                               std::vector<bool>& isArticulation) {
        int children = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;

        for (int v : graph[u]) {
            if (!visited[v]) {
                children++;
                parent[v] = u;

                dfsArticulationPoints(v, disc, low, visited, parent, isArticulation);

                // Check if subtree rooted with v has a connection to
                // an ancestor of u
                low[u] = std::min(low[u], low[v]);

                // Case 1: u is root of DFS tree and has two or more children.
                if (parent[u] == -1 && children > 1) {
                    isArticulation[u] = true;
                }

                // Case 2: If u is not root and low value of one of its child is
                // more than or equal to discovery value of u.
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                }
            }
            // Update low value of u for parent function calls
            else if (v != parent[u]) {
                low[u] = std::min(low[u], disc[v]);
            }
        }
    }
};

int main() {
    // Create a graph with articulation points
    ArticulationPointsFinder network(7);

    // Add edges to create a network
    //     0
    //    / \
    //   1---2
    //  / \   \
    // 3   4   5
    //     |
    //     6
    network.addEdge(0, 1);
    network.addEdge(0, 2);
    network.addEdge(1, 2);
    network.addEdge(1, 3);
    network.addEdge(1, 4);
    network.addEdge(2, 5);
    network.addEdge(4, 6);

    std::vector<int> articulationPoints = network.findArticulationPoints();

    std::cout << "Articulation Points (Critical Nodes): ";
    for (int point : articulationPoints) {
        std::cout << point << " ";
    }
    std::cout << std::endl;

    return 0;
}
