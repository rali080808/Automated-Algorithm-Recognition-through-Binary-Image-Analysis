#include <iostream>
#include <vector>
using namespace std;

class FenwickRangeUpdate {
private:
    vector<int> tree;
    int n;

public:
    FenwickRangeUpdate(int size) {
        n = size;
        tree.assign(n + 1, 0);
    }

    // Helper function for point update
    void update(int idx, int val) {
        while (idx <= n) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    // Update range [left, right] with value val
    void rangeUpdate(int left, int right, int val) {
        update(left, val);
        update(right + 1, -val);
    }

    // Query value at position idx
    int pointQuery(int idx) {
        int result = 0;
        while (idx > 0) {
            result += tree[idx];
            idx -= idx & -idx;
        }
        return result;
    }
};

int main() {
    int n = 10;
    FenwickRangeUpdate rangeTree(n);

    // Initially all elements are 0
    // Add 5 to range [3, 7]
    rangeTree.rangeUpdate(3, 7, 5);

    // Add 2 to range [1, 5]
    rangeTree.rangeUpdate(1, 5, 2);

    // Print final array
    cout << "Final array values: ";
    for (int i = 1; i <= n; i++) {
        cout << rangeTree.pointQuery(i) << " ";
    }
    cout << endl;

    return 0;
}
