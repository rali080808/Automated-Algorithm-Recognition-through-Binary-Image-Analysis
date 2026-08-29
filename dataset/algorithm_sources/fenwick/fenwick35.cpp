// Example 5: Fenwick Tree for Range Minimum Queries
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

class FenwickTreeMin {
private:
    vector<int> bit;
    int size;

public:
    FenwickTreeMin(int n) : size(n) {
        bit.assign(n + 1, INT_MAX);
    }

    void update(int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] = min(bit[idx], val);
    }

    int query(int idx) {
        int minVal = INT_MAX;
        for (; idx > 0; idx -= idx & -idx)
            minVal = min(minVal, bit[idx]);
        return minVal;
    }

    // Reset a position before updating with a new value
    void reset(int idx) {
        // Reset to infinity for this position
        for (; idx <= size; idx += idx & -idx)
            bit[idx] = INT_MAX;
    }
};

int main() {
    vector<int> arr = {5, 2, 9, 1, 7, 3, 8};
    int n = arr.size();

    FenwickTreeMin ft(n);

    // Build the Fenwick tree for minimums
    for (int i = 0; i < n; i++) {
        ft.update(i + 1, arr[i]);
    }

    // Query examples
    cout << "Minimum value from index 1 to 3: " << ft.query(3) << endl;
    cout << "Minimum value from index 1 to 5: " << ft.query(5) << endl;

    // Update value and query again
    ft.reset(4); // Reset before update
    ft.update(4, 0); // Update index 4 with value 0
    cout << "After update, minimum from index 1 to 5: " << ft.query(5) << endl;

    return 0;
}
