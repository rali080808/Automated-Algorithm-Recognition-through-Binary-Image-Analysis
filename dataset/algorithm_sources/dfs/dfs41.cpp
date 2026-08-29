#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>

class IterativeDFS {
private:
    std::vector<std::vector<int>> graph;

public:
    IterativeDFS(int vertices) {
        graph.resize(vertices);
    }

    void addEdge(int from, int to) {
        graph[from].push_back(to);
    }

    void traverseIterative(int start) {
        std::unordered_set<int> visited;
        std::stack<int> nodeStack;

        nodeStack.push(start);

        while (!nodeStack.empty()) {
            int current = nodeStack.top();
            nodeStack.pop();

            if (visited.find(current) != visited.end()) {
                continue;
            }

            visited.insert(current);
            std::cout << "Visiting node: " << current << std::endl;

            // Add neighbors in reverse order to get the same traversal as recursive DFS
            for (auto it = graph[current].rbegin(); it != graph[current].rend(); ++it) {
                if (visited.find(*it) == visited.end()) {
                    nodeStack.push(*it);
                }
            }
        }
    }
};

int main() {
    // Create a graph with 7 vertices
    IterativeDFS graph(7);

    // Add edges
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 4);
    graph.addEdge(2, 5);
    graph.addEdge(2, 6);

    std::cout << "Iterative DFS traversal starting from vertex 0:" << std::endl;
    graph.traverseIterative(0);

    return 0;
}
