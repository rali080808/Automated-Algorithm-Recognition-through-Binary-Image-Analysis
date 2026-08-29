#include <iostream>
#include <vector>
using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;
public:
    FenwickTree(int size) : n(size), bit(size + 2) {}

    void update(int index, int val) {
        while (index < bit.size()) {
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
    FenwickTree ft(100); // Elements in [1, 100]

    ft.update(10, 1);
    ft.update(20, 1);
    ft.update(30, 1);

    cout << "Elements less than 25: " << ft.query(24) << endl;  // Output: 2
    cout << "Elements less than 15: " << ft.query(14) << endl;  // Output: 1
}

