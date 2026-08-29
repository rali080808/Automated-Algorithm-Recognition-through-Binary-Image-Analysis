#include <bits/stdc++.h>
using namespace std;

// Method to calculate prefix sum till index idx
int sum(int idx, vector<int>& F)
{
    int runningSum = 0;
    // Summing up all the partial sums
    while (idx > 0) {
        runningSum += F[idx];
        int rightMostSetBit = (idx & (-idx));
        idx -= rightMostSetBit;
    }
    return runningSum;
}

// Method to update the array by adding X to index idx
void add(int idx, int X, vector<int>& F)
{
    while (idx < F.size()) {
        F[idx] += X;
        int rightMostSetBit = (idx & (-idx));
        idx += rightMostSetBit;
    }
}

// Method to calculate range sum from index l to r
int rangeQuery(int l, int r, vector<int>& F)
{
    return sum(r, F) - sum(l - 1, F);
}

int main()
{
    int n = 5;

    // 1 - based indexing
    vector<int> arr{ -9, 1, 2, 3, 4, 5 };
    // Initially all the values of Fenwick tree are 0
    vector<int> F(n + 1, 0);

    // Build the fenwick tree
    for (int i = 1; i <= n; i++) {
        add(i, arr[i], F);
    }

    // query the sum from index 1 to 3
    cout << rangeQuery(1, 3, F) << "\n";
    // query the sum from index 2 to 5
    cout << rangeQuery(2, 5, F) << "\n";

    // Update element at index i to X
    int i = 3;
    int X = 7;
    // We have passed X - arr[i] to the add method, because
    // add method simply adds a number at a particular index
    // so if we need to update the element, we need to pass
    // the difference between the ith element and X to add
    // method
    add(i, X - arr[i], F);

    // query the sum from index 1 to 3
    cout << rangeQuery(1, 3, F) << "\n";
    // query the sum from 2 to 5
    cout << rangeQuery(2, 5, F) << "\n";

    return 0;
}

