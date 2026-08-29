#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

class EulerianPathFinder {
private:
    std::vector<std::vector<int>> graph;
    std::vector<int> path;
    int numVertices;
    int numEdges;

public:
    EulerianPathFinder(int vertices) : numVertices(vertices), numEdges(0) {
        graph.resize(vertices);
    }

    void addEdge(int u, int v) {
        graph[u].push_back(v);
        numEdges++;
    }

    std::vector<int> findEulerianPath() {
        path.clear();

        // Find a valid starting vertex
        int start = 0;
        std::vector<int> in_degree(numVertices, 0);
        std::vector<int> out_degree(numVertices, 0);

        for (int i = 0; i < numVertices; i++) {
            out_degree[i] = graph[i].size();
            for (int j : graph[i]) {
                in_degree[j]++;
            }
        }

        // Find a starting vertex
        for (int i = 0; i < numVertices; i++) {
            // If out_degree is one more than in_degree, it's the start
            if (out_degree[i] - in_degree[i] == 1) {
                start = i;
                break;
            }
            // If all vertices have balanced in/out degrees, any vertex with
            // edges can be the start
            if (out_degree[i] > 0) {
                start = i;
            }
        }

        // Run DFS from the starting vertex
        dfs(start);

        // Check if we've used all edges
        if (path.size() != numEdges + 1) {
            return {}; // No Eulerian path exists
        }

        // Reverse to get the correct order
        std::reverse(path.begin(), path.end());
        return path;
    }

private:
    void dfs(int vertex) {
        while (!graph[vertex].empty()) {
            int next = graph[vertex].back();
            graph[vertex].pop_back();
            dfs(next);
        }

        path.push_back(vertex);
    }
};

int main() {
    // Example 1: Graph with Eulerian path but no Eulerian circuit
    EulerianPathFinder graph1(5);
    graph1.addEdge(0, 1);
    graph1.addEdge(1, 2);
    graph1.addEdge(2, 0);
    graph1.addEdge(2, 3);
    graph1.addEdge(3, 4);

    std::vector<int> path1 = graph1.findEulerianPath();

    std::cout << "Eulerian path in graph 1: ";
    if (path1.empty()) {
        std::cout << "None exists" << std::endl;
    } else {
        for (int vertex : path1) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    // Example 2: Graph with Eulerian circuit
    EulerianPathFinder graph2(3);
    graph2.addEdge(0, 1);
    graph2.addEdge(1, 2);
    graph2.addEdge(2, 0);

    std::vector<int> path2 = graph2.findEulerianPath();

    std::cout << "Eulerian path in graph 2: ";
    if (path2.empty()) {
        std::cout << "None exists" << std::endl;
    } else {
        for (int vertex : path2) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
