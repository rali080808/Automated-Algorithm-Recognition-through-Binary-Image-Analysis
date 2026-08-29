#include <iostream>
#include <vector>
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

    int query(int index) {
        int res = 0;
        while (index > 0) {
            res += bit[index];
            index -= index & -index;
        }
        return res;
    }

    int findKth(int k) {
        int idx = 0;
        for (int i = 20; i >= 0; --i) {
            int next = idx + (1 << i);
            if (next <= n && bit[next] < k) {
                k -= bit[next];
                idx = next;
            }
        }
        return idx + 1;
    }
};

int main() {
    FenwickTree ft(10);
    ft.update(2, 1);
    ft.update(4, 2);
    ft.update(5, 1);
    ft.update(7, 1);
    cout << ft.findKth(3) << endl;  // Output: 4
}
