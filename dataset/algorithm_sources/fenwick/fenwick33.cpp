// Example 3: Fenwick Tree for Inversion Count
#include <iostream>
#include <vector>
#include <algorithm>
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
};

// Count inversions in array
int countInversions(vector<int>& arr) {
    // Coordinate compression to handle large values
    vector<int> temp = arr;
    sort(temp.begin(), temp.end());
    temp.erase(unique(temp.begin(), temp.end()), temp.end());

    for (int& x : arr) {
        x = lower_bound(temp.begin(), temp.end(), x) - temp.begin() + 1;
    }

    int n = arr.size();
    FenwickTree ft(n);
    int inversions = 0;

    for (int i = n - 1; i >= 0; i--) {
        // Count elements smaller than arr[i] seen so far
        inversions += ft.query(arr[i] - 1);
        ft.update(arr[i], 1);
    }

    return inversions;
}

int main() {
    vector<int> arr = {8, 4, 2, 1};
    cout << "Number of inversions: " << countInversions(arr) << endl;

    vector<int> arr2 = {1, 2, 3, 4};
    cout << "Number of inversions in sorted array: " << countInversions(arr2) << endl;

    vector<int> arr3 = {5, 4, 3, 2, 1};
    cout << "Number of inversions in reverse sorted: " << countInversions(arr3) << endl;

    return 0;
}
