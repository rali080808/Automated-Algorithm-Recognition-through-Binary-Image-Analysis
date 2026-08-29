#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 1) {}

    void rangeAdd(int l, int r, int val) {
        update(l, val);
        update(r + 1, -val);
    }

    void update(int index, int delta) {
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

    ft.rangeAdd(3, 5, 10);  // Adds 10 to all elements from index 3 to 5
    cout << ft.pointQuery(4) << endl;  // Output: 10
    cout << ft.pointQuery(6) << endl;  // Output: 0
}

