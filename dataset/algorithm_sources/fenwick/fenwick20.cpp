#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>
using namespace std;

class VisualFenwickTree {
private:
    vector<int> tree;
    vector<int> original;
    int n;

public:
    VisualFenwickTree(int size) {
        n = size;
        tree.assign(n + 1, 0);
        original.assign(n + 1, 0);
    }

    // Build the Fenwick tree with initial values
    void build(const vector<int>& arr) {
        for (int i = 1; i <= n; i++) {
            original[i] = arr[i];
            update(i, arr[i]);
        }
    }

    // Add value to position idx
    void update(int idx, int value) {
        int delta = value - original[idx];
        original[idx] = value;

        while (idx <= n) {
            tree[idx] += delta;
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

    // Visualize the structure of the Fenwick tree
    void visualizeTree() {
        cout << "\nFenwick Tree Structure:" << endl;
        cout << "----------------------" << endl;

        // Print original array
        cout << "Original array: ";
        for (int i = 1; i <= n; i++) {
            cout << original[i] << " ";
        }
        cout << endl;

        // Print BIT array
        cout << "BIT array:      ";
        for (int i = 1; i <= n; i++) {
            cout << tree[i] << " ";
        }
        cout << endl;

        // Visualize tree structure with coverage
        cout << "\nTree coverage (each node covers a range):" << endl;
        for (int i = 1; i <= n; i++) {
            // Calculate the range covered by this node
            int lsb = i & -i;
            int start = i - lsb + 1;
            int end = i;

            cout << "Node " << setw(2) << i << ": covers [" << setw(2) << start << "," << setw(2) << end
                 << "] = " << setw(3) << tree[i] << " | ";

            // Visual representation
            for (int j = 1; j <= n; j++) {
                if (j >= start && j <= end) {
                    cout << "X";
                } else {
                    cout << ".";
                }
            }
            cout << endl;
        }

        // Visualize query path
        cout << "\nQuery paths for different positions:" << endl;
        for (int pos = 1; pos <= n; pos++) {
            cout << "Query(" << setw(2) << pos << "): ";
            int idx = pos;
            string path = "";
            while (idx > 0) {
                if (!path.empty()) path += " <- ";
                path += to_string(idx);
                idx -= idx & -idx;
            }
            cout << path << endl;
        }

        // Visualize update path
        cout << "\nUpdate paths for different positions:" << endl;
        for (int pos = 1; pos <= n; pos++) {
            cout << "Update(" << setw(2) << pos << "): ";
            int idx = pos;
            string path = "";
            while (idx <= n) {
                if (!path.empty()) path += " -> ";
                path += to_string(idx);
                idx += idx & -idx;
            }
            cout << path << endl;
        }
    }
};

int main() {
    // Example usage
    vector<int> arr = {0, 3, 2, 5, 1, 4, 7, 8}; // 1-indexed array
    int n = arr.size() - 1;

    VisualFenwickTree bit(n);
    bit.build(arr);

    // Visualize the Fenwick tree structure
    bit.visualizeTree();

    // Example queries
    cout << "\nExample Queries:" << endl;
    cout << "Sum of range [2, 5]: " << bit.rangeQuery(2, 5) << endl;
    cout << "Sum of range [1, 7]: " << bit.rangeQuery(1, 7) << endl;

    // Update a value
    cout << "\nUpdating value at position 3 from " << arr[3] << " to 10" << endl;
    bit.update(3, 10);

    // Visualize after update
    bit.visualizeTree();

    // Example queries after update
    cout << "\nExample Queries After Update:" << endl;
    cout << "Sum of range [2, 5]: " << bit.rangeQuery(2, 5) << endl;
    cout << "Sum of range [1, 7]: " << bit.rangeQuery(1, 7) << endl;

    return 0;
}
