#include <iostream>
#include <unordered_map>
using namespace std;

class LazyFenwickTree {
private:
    unordered_map<int, int> tree;
    int n;

    // Get the parent of node idx
    int parent(int idx) {
        return idx - (idx & -idx);
    }

    // Get the next node in update path
    int next(int idx) {
        return idx + (idx & -idx);
    }

public:
    LazyFenwickTree(int size) {
        n = size;
    }

    void update(int idx, int value) {
        if (idx <= 0 || idx > n) return;

        while (idx <= n) {
            tree[idx] += value;
            idx = next(idx);
        }
    }

    int query(int idx) {
        if (idx <= 0) return 0;
        if (idx > n) idx = n;

        int sum = 0;
        while (idx > 0) {
            if (tree.find(idx) != tree.end()) {
                sum += tree[idx];
            }
            idx = parent(idx);
        }
        return sum;
    }

    int rangeQuery(int left, int right) {
        return query(right) - query(left - 1);
    }

    // Get value at a specific index
    int get(int idx) {
        return rangeQuery(idx, idx);
    }
};

int main() {
    // This version works well for sparse arrays
    LazyFenwickTree sparseTree(1000000);  // Very large size, but memory efficient

    // Update just a few positions
    sparseTree.update(100, 5);
    sparseTree.update(10000, 10);
    sparseTree.update(500000, 15);

    cout << "Value at position 100: " << sparseTree.get(100) << endl;
    cout << "Sum of range [100, 10000]: " << sparseTree.rangeQuery(100, 10000) << endl;
    cout << "Sum of range [1, 500000]: " << sparseTree.rangeQuery(1, 500000) << endl;

    // Update existing position
    sparseTree.update(100, 7);
    cout << "Value at position 100 after update: " << sparseTree.get(100) << endl;

    return 0;
}
