#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 1, INT_MIN) {}

    void update(int index, int val) {
        while (index <= n) {
            bit[index] = max(bit[index], val);
            index += index & -index;
        }
    }

    int query(int index) {
        int res = INT_MIN;
        while (index > 0) {
            res = max(res, bit[index]);
            index -= index & -index;
        }
        return res;
    }
};

int main() {
    FenwickTree ft(10);

    ft.update(3, 5);
    ft.update(7, 8);
    cout << "Maximum value in the range [1, 7]: " << ft.query(7) << endl;  // Output: 8
    cout << "Maximum value in the range [1, 3]: " << ft.query(3) << endl;  // Output: 5
}

