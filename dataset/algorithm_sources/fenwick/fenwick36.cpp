// Example 6: Fenwick Tree for Range Maximum Queries
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

class FenwickTreeMax {
private:
    vector<int> bit;
    int size;

public:
    FenwickTreeMax(int n) : size(n) {
        bit.assign(n + 1, INT_MIN);
    }

    void update(int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] = max(bit[idx], val);
    }

    int query(int idx) {
        int maxVal = INT_MIN;
        for (; idx > 0; idx -= idx & -idx)
            maxVal = max(maxVal, bit[idx]);
        return maxVal;
    }

    // Reset a position before updating with a new value
    void reset(int idx) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] = INT_MIN;
    }
};

int main() {
    vector<int> arr = {5, 2, 9, 1, 7, 3, 8};
    int n = arr.size();

    FenwickTreeMax ft(n);

    // Build the Fenwick tree for maximums
    for (int i = 0; i < n; i++) {
        ft.update(i + 1, arr[i]);
    }

    // Query examples
    cout << "Maximum value from index 1 to 3: " << ft.query(3) << endl;
    cout << "Maximum value from index 1 to 5: " << ft.query(5) << endl;

    // Update value and query again
    ft.reset(3); // Reset before update
    ft.update(3, 15); // Update index 3 with value 15
    cout << "After update, maximum from index 1 to 5: " << ft.query(5) << endl;

    return 0;
}
