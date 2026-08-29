#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>

class TopologicalSort {
private:
    std::vector<std::vector<int>> graph;
    int numVertices;

public:
    TopologicalSort(int vertices) : numVertices(vertices) {
        graph.resize(vertices);
    }

    void addEdge(int from, int to) {
        graph[from].push_back(to);
    }

    std::vector<int> sort() {
        std::vector<bool> visited(numVertices, false);
        std::vector<bool> inRecursionStack(numVertices, false);
        std::stack<int> topologicalOrder;
        std::vector<int> result;

        // Check for cycles first (optional)
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                if (hasCycle(i, visited, inRecursionStack)) {
                    std::cout << "Graph has a cycle. Topological sort not possible." << std::endl;
                    return {};
                }
            }
        }

        // Reset visited array
        std::fill(visited.begin(), visited.end(), false);

        // Perform topological sort
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, topologicalOrder);
            }
        }

        // Convert stack to vector
        while (!topologicalOrder.empty()) {
            result.push_back(topologicalOrder.top());
            topologicalOrder.pop();
        }

        return result;
    }

private:
    void topologicalSortUtil(int vertex, std::vector<bool>& visited, std::stack<int>& stack) {
        visited[vertex] = true;

        for (int adjacent : graph[vertex]) {
            if (!visited[adjacent]) {
                topologicalSortUtil(adjacent, visited, stack);
            }
        }

        // Push current vertex to stack after all its adjacent vertices are processed
        stack.push(vertex);
    }

    bool hasCycle(int vertex, std::vector<bool>& visited, std::vector<bool>& inStack) {
        visited[vertex] = true;
        inStack[vertex] = true;

        for (int adjacent : graph[vertex]) {
            if (!visited[adjacent] && hasCycle(adjacent, visited, inStack)) {
                return true;
            } else if (inStack[adjacent]) {
                return true;
            }
        }

        inStack[vertex] = false;
        return false;
    }
};

int main() {
    // Create a directed acyclic graph for course prerequisites
    TopologicalSort courseGraph(6);

    // Add edges representing prerequisites
    // Course 5 depends on 2 and 0
    // Course 4 depends on 1
    // Course 3 depends on 1
    // Course 2 depends on 0
    courseGraph.addEdge(0, 2);
    courseGraph.addEdge(0, 5);
    courseGraph.addEdge(1, 3);
    courseGraph.addEdge(1, 4);
    courseGraph.addEdge(2, 5);

    std::vector<int> order = courseGraph.sort();

    if (!order.empty()) {
        std::cout << "Topological Sort (Course Order): ";
        for (int course : order) {
            std::cout << course << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
