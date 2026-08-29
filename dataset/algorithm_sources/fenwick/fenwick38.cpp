// Example 9: Fenwick Tree for Counting Distinct Elements in Range
#include <iostream>
#include <vector>
#include <map>
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

    int rangeQuery(int l, int r) {
        return query(r) - query(l - 1);
    }
};

int main() {
    vector<int> arr = {1, 2, 1, 3, 2, 4, 5, 1};
    int n = arr.size();

    // For each element, store positions of its occurrences
    map<int, vector<int>> positions;
    for (int i = 0; i < n; i++) {
        positions[arr[i]].push_back(i + 1);  // 1-indexed
    }

    // Process offline queries
    vector<pair<int, int>> queries = {{1, 5}, {2, 7}, {3, 8}, {1, 8}};

    // For each query, count distinct elements in range
    for (auto &q : queries) {
        int l = q.first;
        int r = q.second;

        FenwickTree ft(n);
        int distinct = 0;

        // For each unique element
        for (auto &p : positions) {
            vector<int> &pos = p.second;

            // Find position in range
            auto it = lower_bound(pos.begin(), pos.end(), l);
            if (it != pos.end() && *it <= r) {
                distinct++;

                // Add all positions in range to Fenwick tree
                for (auto it2 = it; it2 != pos.end() && *it2 <= r; ++it2) {
                    ft.update(*it2, 1);
                }
            }
        }

        cout << "Distinct elements in range [" << l << ", " << r << "]: " << distinct << endl;
        cout << "Total elements in range: " << ft.rangeQuery(l, r) << endl;
    }

    return 0;
}
