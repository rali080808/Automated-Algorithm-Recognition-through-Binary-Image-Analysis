#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

// Dynamic Fenwick Tree that doesn't require a fixed size
// and can handle arbitrary indices (positive integers)
class DynamicFenwick {
private:
    map<int, int> tree;
    set<int> indices;
    int maxIdx;

    int getLSB(int idx) {
        return idx & -idx;
    }

    // Find largest value <= x in the set
    int floorIndex(int x) {
        auto it = indices.upper_bound(x);
        if (it == indices.begin()) return 0; // No floor exists
        --it;
        return *it;
    }

public:
    DynamicFenwick() : maxIdx(0) {}

    // Update a position with a value
    void update(int idx, int val) {
        if (idx <= 0) return;

        indices.insert(idx);
        maxIdx = max(maxIdx, idx);

        while (idx <= maxIdx) {
            tree[idx] += val;

            // Find the next index in the Fenwick tree path
            int nextIdx = idx + getLSB(idx);

            // If the next index doesn't exist, we need to create it
            if (nextIdx <= maxIdx) {
                indices.insert(nextIdx);
            }

            idx = nextIdx;
        }
    }

    // Query sum from 1 to idx
    int query(int idx) {
        if (idx <= 0) return 0;

        int sum = 0;
        while (idx > 0) {
            // If this index exists in our tree, add its value
            if (indices.count(idx) > 0) {
                sum += tree[idx];
            }

            // Get the parent in the Fenwick tree
            idx -= getLSB(idx);
        }

        return sum;
    }

    // Get sum in range [left, right]
    int rangeQuery(int left, int right) {
        if (left > right) return 0;
        return query(right) - query(left - 1);
    }

    // Get the value at a specific index
    int getValue(int idx) {
        return rangeQuery(idx, idx);
    }

    // Check if an index exists in the tree
    bool exists(int idx) {
        return indices.count(idx) > 0;
    }

    // Print information about the tree
    void printInfo() {
        cout << "Dynamic Fenwick Tree Information:" << endl;
        cout << "Number of indices: " << indices.size() << endl;
        cout << "Maximum index: " << maxIdx << endl;

        cout << "Existing indices: ";
        for (int idx : indices) {
            cout << idx << " ";
        }
        cout << endl;

        cout << "Values at each index:" << endl;
        for (int i = 1; i <= maxIdx; i++) {
            if (exists(i)) {
                cout << "Index " << i << ": " << getValue(i) << endl;
            }
        }
    }
};

// Function to simulate a sparse array operation
void simulateSparseArray() {
    // Let's say we're tracking data points that are very far apart
    DynamicFenwick sparseArray;

    // Update some values at arbitrarily spaced indices
    sparseArray.update(10, 5);       // Position 10 has value 5
    sparseArray.update(1000, 15);    // Position 1000 has value 15
    sparseArray.update(10000, 25);   // Position 10000 has value 25
    sparseArray.update(100000, 35);  // Position 100000 has value 35

    // Query sums
    cout << "Sum from 1 to 1000: " << sparseArray.query(1000) << endl;
    cout << "Sum from 1 to 50000: " << sparseArray.query(50000) << endl;

    // Get a range sum
    cout << "Sum from 1000 to 10000: " << sparseArray.rangeQuery(1000, 10000) << endl;

    // Get individual values
    cout << "Value at index 10: " << sparseArray.getValue(10) << endl;
    cout << "Value at index 1000: " << sparseArray.getValue(1000) << endl;
    cout << "Value at index 5000: " << sparseArray.getValue(5000) << endl; // Should be 0

    // Print information about the sparse array
    sparseArray.printInfo();
}

int main() {
    cout << "=== Dynamic Fenwick Tree Example ===" << endl;

    // Create a dynamic Fenwick tree
    DynamicFenwick dynTree;

    // Insert some values
    dynTree.update(3, 5);
    dynTree.update(7, 10);
    dynTree.update(1, 3);
    dynTree.update(12, 8);
    dynTree.update(5, 6);

    // Query some ranges
    cout << "Sum from 1 to 5: " << dynTree.query(5) << endl;
    cout << "Sum from 1 to 10: " << dynTree.query(10) << endl;
    cout << "Sum from 3 to 7: " << dynTree.rangeQuery(3, 7) << endl;

    // Update an existing value
    cout << "\nUpdating index 3 with +7..." << endl;
    dynTree.update(3, 7);

    // Query again
    cout << "Sum from 1 to 5 after update: " << dynTree.query(5) << endl;
    cout << "Sum from 3 to 7 after update: " << dynTree.rangeQuery(3, 7) << endl;

    // Print information about the tree
    dynTree.printInfo();

    // Example of a sparse array use case
    cout << "\n=== Sparse Array Simulation ===" << endl;
    simulateSparseArray();

    return 0;
}
