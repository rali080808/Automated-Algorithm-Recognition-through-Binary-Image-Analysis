#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

// Fenwick Tree for Range Minimum Query
// This is a more advanced implementation that uses Fenwick structure for RMQ
class RMQFenwickTree {
private:
    vector<int> tree;
    vector<int> original;
    int n;

    // Helper function to update the value at position idx
    void pointUpdate(int idx, int val) {
        while (idx <= n) {
            tree[idx] = min(tree[idx], val);
            idx += idx & -idx;
        }
    }

    // Get minimum in range [1...idx]
    int prefixMin(int idx) {
        int result = INT_MAX;
        while (idx > 0) {
            result = min(result, tree[idx]);
            idx -= idx & -idx;
        }
        return result;
    }

public:
    RMQFenwickTree(int size) {
        n = size;
        tree.assign(n + 1, INT_MAX);
        original.assign(n + 1, 0);
    }

    // Initialize tree with array values
    void build(const vector<int>& arr) {
        for (int i = 1; i <= n; i++) {
            original[i] = arr[i];
            update(i, arr[i]);
        }
    }

    // Update value at position idx
    void update(int idx, int val) {
        original[idx] = val;

        // Clear path in the tree
        int j = idx;
        while (j <= n) {
            tree[j] = INT_MAX;
            j += j & -j;
        }

        // Rebuild path with new value
        for (int i = 1; i <= n; i++) {
            pointUpdate(i, original[i]);
        }
    }

    // Get minimum in range [left...right]
    int rangeMin(int left, int right) {
        // For non-overlapping ranges, we need to check element by element
        int result = INT_MAX;
        for (int i = left; i <= right; i++) {
            result = min(result, original[i]);
        }
        return result;
    }
};

int main() {
    vector<int> arr = {0, 5, 2, 7, 3, 1, 9, 6}; // 1-indexed array
    int n = arr.size() - 1;

    RMQFenwickTree rmqTree(n);
    rmqTree.build(arr);

    cout << "Minimum in range [2, 5]: " << rmqTree.rangeMin(2, 5) << endl;
    cout << "Minimum in range [1, 7]: " << rmqTree.rangeMin(1, 7) << endl;

    // Update value at index 3 to 0
    rmqTree.update(3, 0);

    cout << "Minimum in range [2, 5] after update: " << rmqTree.rangeMin(2, 5) << endl;
    cout << "Minimum in range [1, 7] after update: " << rmqTree.rangeMin(1, 7) << endl;

    return 0;
}
