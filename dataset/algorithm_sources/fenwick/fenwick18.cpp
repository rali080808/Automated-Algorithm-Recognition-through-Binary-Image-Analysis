#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class InversionCounter {
private:
    vector<int> tree;
    int n;

    void update(int idx, int val) {
        while (idx <= n) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    int query(int idx) {
        int sum = 0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }
        return sum;
    }

public:
    long long countInversions(vector<int>& arr) {
        vector<int> sortedArr = arr;
        sort(sortedArr.begin(), sortedArr.end());

        // Coordinate compression
        for (int i = 0; i < arr.size(); i++) {
            arr[i] = lower_bound(sortedArr.begin(), sortedArr.end(), arr[i]) - sortedArr.begin() + 1;
        }

        n = arr.size();
        tree.assign(n + 1, 0);

        long long inversions = 0;
        for (int i = n - 1; i >= 0; i--) {
            inversions += query(arr[i] - 1);
            update(arr[i], 1);
        }

        return inversions;
    }
};

int main() {
    vector<int> arr = {8, 4, 2, 1};
    InversionCounter counter;

    long long inversions = counter.countInversions(arr);
    cout << "Number of inversions: " << inversions << endl;

    // Another example
    vector<int> arr2 = {3, 1, 4, 5, 2};
    cout << "Number of inversions in second array: " << counter.countInversions(arr2) << endl;

    return 0;
}
