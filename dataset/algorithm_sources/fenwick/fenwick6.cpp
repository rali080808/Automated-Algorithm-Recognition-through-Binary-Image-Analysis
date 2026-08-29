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
};

int main() {
    int n = 10;
    FenwickTree ft(n);

    // Let's assume array elements are from 1 to 10, and we want to count their occurrences.
    ft.update(2, 1);
    ft.update(5, 1);
    ft.update(2, 1);
    ft.update(7, 1);

    cout << "Frequency of 2: " << ft.query(2) << endl;  // Output: 2
    cout << "Frequency of 5: " << ft.query(5) << endl;  // Output: 1
}

