// Example 1: Basic Fenwick Tree for Range Sum Queries
#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
private:
    vector<int> bit;
    int size;

public:
    FenwickTree(int n) : size(n) {
        bit.assign(n + 1, 0);
    }

    void update(int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] += val;
    }

    int query(int idx) {
        int sum = 0;
        for (; idx > 0; idx -= idx & -idx)
            sum += bit[idx];
        return sum;
    }

    int rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    int n = arr.size();

    FenwickTree ft(n);

    // Build the Fenwick tree
    for (int i = 0; i < n; i++) {
        ft.update(i + 1, arr[i]);
    }

    // Query examples
    cout << "Sum of elements from index 1 to 3: " << ft.rangeQuery(1, 3) << endl;
    cout << "Sum of elements from index 2 to 5: " << ft.rangeQuery(2, 5) << endl;

    // Update and query again
    ft.update(3, 2); // Add 2 to element at index 3
    cout << "After update, sum from index 1 to 3: " << ft.rangeQuery(1, 3) << endl;

    return 0;
}
