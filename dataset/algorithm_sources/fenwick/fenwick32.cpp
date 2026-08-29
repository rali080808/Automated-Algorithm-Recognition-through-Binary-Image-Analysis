// Example 2: Fenwick Tree to find frequency of elements in array
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class FenwickTree {
private:
    vector<int> bit;
    int size;

public:
    FenwickTree(int n) : size(n) {
        bit.assign(n + 1, 0);
    }

    void update(int idx, int val) {
        for (; idx <= size; idx += idx & -idx)
            bit[idx] += val;
    }

    int query(int idx) {
        int sum = 0;
        for (; idx > 0; idx -= idx & -idx)
            sum += bit[idx];
        return sum;
    }
};

int main() {
    vector<int> arr = {1, 5, 2, 5, 3, 1, 5, 7};
    int maxVal = *max_element(arr.begin(), arr.end());

    FenwickTree ft(maxVal);

    // Count frequencies of each element
    for (int x : arr) {
        ft.update(x, 1);
    }

    // Print frequency of each number
    for (int i = 1; i <= maxVal; i++) {
        int freq = ft.query(i) - ft.query(i - 1);
        if (freq > 0) {
            cout << "Frequency of " << i << ": " << freq << endl;
        }
    }

    // Add more occurrences
    ft.update(3, 2); // Add 2 more occurrences of number 3
    cout << "After update, frequency of 3: " << (ft.query(3) - ft.query(2)) << endl;

    return 0;
}
