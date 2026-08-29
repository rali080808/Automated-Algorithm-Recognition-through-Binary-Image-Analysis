#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 1) {}

    void update(int index, int val) {
        while (index <= n) {
            bit[index] += val;
            index += index & -index;
        }
    }

    int prefixSum(int index) {
        int res = 0;
        while (index > 0) {
            res += bit[index];
            index -= index & -index;
        }
        return res;
    }
};

int countInversions(vector<int>& arr) {
    int n = arr.size();
    vector<int> sorted(arr);
    sort(sorted.begin(), sorted.end());

    FenwickTree ft(n);
    int inv_count = 0;

    for (int i = n - 1; i >= 0; --i) {
        int rank = lower_bound(sorted.begin(), sorted.end(), arr[i]) - sorted.begin() + 1;
        inv_count += ft.prefixSum(rank - 1);
        ft.update(rank, 1);
    }
    return inv_count;
}

int main() {
    vector<int> arr = {3, 1, 2};
    cout << countInversions(arr) << endl;  // Output: 2
}
