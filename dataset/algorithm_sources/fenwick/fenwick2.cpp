#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 1) {}

    void rangeAdd(int l, int r, int val) {
        add(l, val);
        add(r + 1, -val);
    }

    void add(int index, int delta) {
        while (index <= n) {
            bit[index] += delta;
            index += index & -index;
        }
    }

    int pointQuery(int index) {
        int res = 0;
        while (index > 0) {
            res += bit[index];
            index -= index & -index;
        }
        return res;
    }
};

int main() {
    FenwickTree ft(10);
    ft.rangeAdd(2, 5, 10);
    cout << ft.pointQuery(3) << endl;  // Output: 10
    cout << ft.pointQuery(6) << endl;  // Output: 0
}
