// Example 7: Fenwick Tree for Range Updates (Point Queries)
#include <iostream>
#include <vector>
using namespace std;

class FenwickTreeRangeUpdate {
private:
    vector<int> bit;
    int size;

public:
    FenwickTreeRangeUpdate(int n) : size(n) {
        bit.assign(n + 1, 0);
    }

    // Update single position
    void update(int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] += val;
    }

    // Update range [l, r] with value val
    void rangeUpdate(int l, int r, int val) {
        update(l, val);
        update(r + 1, -val);
    }

    // Get value at position idx
    int pointQuery(int idx) {
        int result = 0;
        for (; idx > 0; idx -= idx & -idx)
            result += bit[idx];
        return result;
    }
};

int main() {
    int n = 10;
    FenwickTreeRangeUpdate ft(n);

    // Initial array is all zeros

    // Update ranges
    ft.rangeUpdate(1, 5, 3);  // Add 3 to range [1, 5]
    ft.rangeUpdate(3, 8, 2);  // Add 2 to range [3, 8]

    // Query individual positions
    cout << "Values after range updates:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "Position " << i << ": " << ft.pointQuery(i) << endl;
    }

    // Another range update
    ft.rangeUpdate(4, 6, -1); // Subtract 1 from range [4, 6]

    cout << "\nValues after another range update:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "Position " << i << ": " << ft.pointQuery(i) << endl;
    }

    return 0;
}
