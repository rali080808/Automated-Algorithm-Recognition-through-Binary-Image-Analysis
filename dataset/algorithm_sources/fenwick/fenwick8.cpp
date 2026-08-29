
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 1, INT_MAX) {}

    void update(int index, int val) {
        while (index <= n) {
            bit[index] = min(bit[index], val);
            index += index & -index;
        }
    }

    int query(int index) {
        int res = INT_MAX;
        while (index > 0) {
            res = min(res, bit[index]);
            index -= index & -index;
        }
        return res;
    }
};

int main() {
    FenwickTree ft(10);

    ft.update(3, 5);
    ft.update(7, 2);
    cout << "Minimum value in the range [1, 5]: " << ft.query(5) << endl;  // Output: 5
    cout << "Minimum value in the range [1, 7]: " << ft.query(7) << endl;  // Output: 2
}

