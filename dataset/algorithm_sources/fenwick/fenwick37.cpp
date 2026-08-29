// Example 8: Fenwick Tree for Range Updates and Range Queries
#include <iostream>
#include <vector>
using namespace std;

class FenwickTreeRUR {
private:
    vector<int> BIT1, BIT2;
    int size;

public:
    FenwickTreeRUR(int n) : size(n) {
        BIT1.assign(n + 1, 0);
        BIT2.assign(n + 1, 0);
    }

    void update(vector<int>& bit, int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] += val;
    }

    int query(vector<int>& bit, int idx) {
        int sum = 0;
        for (; idx > 0; idx -= idx & -idx)
            sum += bit[idx];
        return sum;
    }

    // Range update: Add val to all elements in range [l, r]
    void rangeUpdate(int l, int r, int val) {
        update(BIT1, l, val);
        update(BIT1, r+1, -val);
        update(BIT2, l, val * (l-1));
        update(BIT2, r+1, -val * r);
    }

    // Get sum up to position idx
    int prefixSum(int idx) {
        return query(BIT1, idx) * idx - query(BIT2, idx);
    }

    // Get sum of range [l, r]
    int rangeSum(int l, int r) {
        return prefixSum(r) - prefixSum(l-1);
    }
};

int main() {
    int n = 10;
    FenwickTreeRUR ft(n);

    // Initial array is all zeros

    // Update ranges
    ft.rangeUpdate(1, 5, 3);  // Add 3 to range [1, 5]
    ft.rangeUpdate(3, 8, 2);  // Add 2 to range [3, 8]

    // Query ranges
    cout << "Sum of range [1, 5]: " << ft.rangeSum(1, 5) << endl;
    cout << "Sum of range [3, 8]: " << ft.rangeSum(3, 8) << endl;
    cout << "Sum of range [2, 7]: " << ft.rangeSum(2, 7) << endl;

    // Another range update
    ft.rangeUpdate(4, 6, -1); // Subtract 1 from range [4, 6]

    cout << "\nAfter another range update:" << endl;
    cout << "Sum of range [1, 5]: " << ft.rangeSum(1, 5) << endl;
    cout << "Sum of range [3, 8]: " << ft.rangeSum(3, 8) << endl;
    cout << "Sum of range [2, 7]: " << ft.rangeSum(2, 7) << endl;

    return 0;
}
