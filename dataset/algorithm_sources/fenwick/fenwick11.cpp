#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(vector<int>& arr) {
        n = arr.size();
        bit.assign(n + 1, 0);
        for (int i = 0; i < n; ++i)
            update(i + 1, arr[i] - (i > 0 ? arr[i-1] : 0));
    }

    void update(int index, int val) {
        while (index <= n) {
            bit[index] += val;
            index += index & -index;
        }
    }

    int query(int index) {
        int res = 0;
        while (index > 0) {
            res += bit[index];
            index -= index & -index;
        }
        return res;
    }
};

int main() {
    vector<int> prefixSum = {2, 5, 9, 10}; // Original was [2,3,4,1]
    FenwickTree ft(prefixSum);

    for (int i = 1; i <= prefixSum.size(); ++i) {
        cout << "Element " << i << ": " << ft.query(i) - ft.query(i-1) << endl;
    }
}

