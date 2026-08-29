#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
private:
    vector<int> tree;
    int n;

public:
    FenwickTree(int size) {
        n = size;
        tree.assign(n + 1, 0);
    }

    // Add value to position idx
    void update(int idx, int value) {
        while (idx <= n) {
            tree[idx] += value;
            idx += idx & -idx; // Add least significant bit
        }
    }

    // Get sum of range [1...idx]
    int query(int idx) {
        int sum = 0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx; // Remove least significant bit
        }
        return sum;
    }

    // Get sum of range [left...right]
    int rangeQuery(int left, int right) {
        return query(right) - query(left - 1);
    }
};

int main() {
    // Example usage
    vector<int> arr = {0, 3, 2, 5, 1, 4}; // 1-indexed array
    int n = arr.size() - 1;

    FenwickTree bit(n);

    // Build the Fenwick tree
    for (int i = 1; i <= n; i++) {
        bit.update(i, arr[i]);
    }

    cout << "Sum of range [2, 5]: " << bit.rangeQuery(2, 5) << endl;

    // Update value at index 3 to 8 (delta = 3)
    bit.update(3, 3);

    cout << "Sum of range [2, 5] after update: " << bit.rangeQuery(2, 5) << endl;

    return 0;
}
