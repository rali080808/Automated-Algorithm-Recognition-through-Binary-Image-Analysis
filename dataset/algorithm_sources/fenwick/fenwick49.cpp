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

// Method to fetch element at index idx
int pointQuery(int idx, vector<int>& F)
{
    return sum(idx, F);
}

// Add X to all elements in range [l, r]
void rangeUpdate(int l, int r, int X, vector<int> &F) {
    add(l, X, F);
    add(r + 1, -X, F);
}

int main()
{
    int n = 5;

    // 1 - based indexing
    vector<int> arr{ -9, 1, 32, 323, 4, 52233 };

    // Initially all the values of Fenwick tree are 0
    vector<int> F(n + 1, 0);

    // Build the fenwick tree
    for (int i = 1; i <= n; i++) {
        rangeUpdate(i, i, arr[i], F);
    }

    // fetch the element at index 2
    cout << pointQuery(2, F) << "\n";
    // fetch the element at index 4
    cout << pointQuery(4, F) << "\n";

    // Add X to all the elements from l to r
    int l = 2, r = 4;
    int X = 7;

    rangeUpdate(l, r, X, F);

    // fetch the element at index 2
    cout << pointQuery(2, F) << "\n";
    // fetch the element at index 4
    cout << pointQuery(4, F) << "\n";

    return 0;
}

