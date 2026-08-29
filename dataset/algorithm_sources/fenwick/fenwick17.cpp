#include <iostream>
#include <vector>
using namespace std;

class XORFenwickTree {
private:
    vector<int> tree;
    int n;

public:
    XORFenwickTree(int size) {
        n = size;
        tree.assign(n + 1, 0);
    }

    void update(int idx, int value) {
        while (idx <= n) {
            tree[idx] ^= value;
            idx += idx & -idx;
        }
    }

    int query(int idx) {
        int result = 0;
        while (idx > 0) {
            result ^= tree[idx];
            idx -= idx & -idx;
        }
        return result;
    }

    int rangeQuery(int left, int right) {
        return query(right) ^ query(left - 1);
    }
};

int main() {
    vector<int> arr = {0, 3, 7, 4, 9, 2, 6}; // 1-indexed array
    int n = arr.size() - 1;

    XORFenwickTree xorTree(n);

    // Build the XOR Fenwick tree
    for (int i = 1; i <= n; i++) {
        xorTree.update(i, arr[i]);
    }

    cout << "XOR of range [2, 5]: " << xorTree.rangeQuery(2, 5) << endl;

    // Update value at index 3 from 4 to 8
    xorTree.update(3, arr[3]); // Undo the old value with XOR
    arr[3] = 8;
    xorTree.update(3, arr[3]); // Add the new value with XOR

    cout << "XOR of range [2, 5] after update: " << xorTree.rangeQuery(2, 5) << endl;

    // Print all values
    cout << "Individual values after update: ";
    for (int i = 1; i <= n; i++) {
        int value = xorTree.rangeQuery(i, i);
        cout << value << " ";
    }
    cout << endl;

    return 0;
}
