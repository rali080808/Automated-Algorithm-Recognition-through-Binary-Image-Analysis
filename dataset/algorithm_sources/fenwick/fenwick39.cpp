// Example 10: Fenwick Tree for Dynamic Median Finding
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

    // Find k-th element (binary search on the prefix sums)
    int findKth(int k) {
        int low = 1, high = size;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (query(mid) < k)
                low = mid + 1;
            else
                high = mid;
        }
        return low;
    }
};

int main() {
    vector<int> arr = {5, 2, 9, 1, 7, 3, 8, 6, 4};

    // Coordinate compression
    vector<int> sorted = arr;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());

    // Map original values to ranks
    vector<int> ranks(arr.size());
    for (int i = 0; i < arr.size(); i++) {
        ranks[i] = lower_bound(sorted.begin(), sorted.end(), arr[i]) - sorted.begin() + 1;
    }

    FenwickTree ft(sorted.size());

    cout << "Dynamic Median Finding:" << endl;
    for (int i = 0; i < ranks.size(); i++) {
        // Insert current element
        ft.update(ranks[i], 1);

        // Find median of elements inserted so far
        int medianRank;
        int count = i + 1;

        if (count % 2 == 1) {
            // Odd count, single median
            medianRank = ft.findKth((count + 1) / 2);
            cout << "After inserting " << arr[i] << ", median is: " << sorted[medianRank - 1] << endl;
        } else {
            // Even count, average of two medians
            int rank1 = ft.findKth(count / 2);
            int rank2 = ft.findKth(count / 2 + 1);
            double median = (sorted[rank1 - 1] + sorted[rank2 - 1]) / 2.0;
            cout << "After inserting " << arr[i] << ", median is: " << median << endl;
        }
    }

    return 0;
}
