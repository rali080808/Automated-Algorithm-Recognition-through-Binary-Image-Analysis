#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <utility> // For std::pair
using namespace std;

// Fenwick Tree to solve Maximum Subarray Sum with Updates
class MaxSubarrayFenwick {
private:
    vector<int> array;      // Original array
    vector<int> prefix_sum; // Fenwick tree for prefix sums
    vector<int> best_sum;   // Store maximum subarray sum ending at each position
    int n;

    // Update Fenwick tree
    void updateFenwick(vector<int>& bit, int idx, int val) {
        while (idx <= n) {
            bit[idx] += val;
            idx += idx & -idx;
        }
    }

    // Query Fenwick tree for sum from 1 to idx
    int queryFenwick(vector<int>& bit, int idx) {
        int res = 0;
        while (idx > 0) {
            res += bit[idx];
            idx -= idx & -idx;
        }
        return res;
    }

    // Recompute best_sum array after an update
    void recomputeBestSum() {
        best_sum.assign(n + 1, 0);
        int min_prefix = 0;
        int curr_max = INT_MIN;

        for (int i = 1; i <= n; i++) {
            int curr_prefix = queryFenwick(prefix_sum, i);
            curr_max = max(curr_max, curr_prefix - min_prefix);
            min_prefix = min(min_prefix, curr_prefix);
            best_sum[i] = curr_max;
        }
    }

public:
    MaxSubarrayFenwick(int size) {
        n = size;
        array.assign(n + 1, 0);
        prefix_sum.assign(n + 1, 0);
        best_sum.assign(n + 1, 0);
    }

    // Initialize with array values
    void build(const vector<int>& arr) {
        for (int i = 1; i <= n; i++) {
            update(i, arr[i]);
        }
    }

    // Update value at position idx
    void update(int idx, int val) {
        int delta = val - array[idx];
        array[idx] = val;

        // Update prefix sum Fenwick tree
        updateFenwick(prefix_sum, idx, delta);

        // Recompute maximum subarray sums
        recomputeBestSum();
    }

    // Get maximum subarray sum ending at or before position idx
    int maxSubarraySum(int idx) {
        if (idx <= 0) return 0;
        if (idx > n) idx = n;
        return best_sum[idx];
    }

    // Get maximum subarray sum in the entire array
    int maxSubarraySum() {
        return maxSubarraySum(n);
    }

    // Get the start and end indices of maximum subarray
    pair<int, int> maxSubarrayIndices() {
        int max_sum = INT_MIN;
        int min_prefix = 0;
        int min_idx = 0;
        int end_idx = 0;

        for (int i = 1; i <= n; i++) {
            int curr_prefix = queryFenwick(prefix_sum, i);

            if (curr_prefix - min_prefix > max_sum) {
                max_sum = curr_prefix - min_prefix;
                end_idx = i;
            }

            if (curr_prefix < min_prefix) {
                min_prefix = curr_prefix;
                min_idx = i;
            }
        }

        // Find exact start index by scanning backward
        int start_idx = 1;
        for (int i = min_idx + 1; i <= end_idx; i++) {
            int curr_sum = queryFenwick(prefix_sum, end_idx) - queryFenwick(prefix_sum, i - 1);
            if (curr_sum == max_sum) {
                start_idx = i;
                break;
            }
        }

        return make_pair(start_idx, end_idx);
    }

    // Print current array
    void printArray() {
        cout << "Current array: ";
        for (int i = 1; i <= n; i++) {
            cout << array[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    vector<int> arr = {0, -2, 1, -3, 4, -1, 2, 1, -5, 4}; // 1-indexed
    int n = arr.size() - 1;

    MaxSubarrayFenwick msf(n);
    msf.build(arr);

    cout << "Original array:" << endl;
    msf.printArray();

    int max_sum = msf.maxSubarraySum();
    pair<int, int> indices = msf.maxSubarrayIndices();
    int start = indices.first;
    int end = indices.second;

    cout << "Maximum subarray sum: " << max_sum << endl;
    cout << "Maximum subarray: [" << start << ", " << end << "]" << endl;

    // Update some values
    cout << "\nUpdating value at position 4 from " << arr[4] << " to 5" << endl;
    msf.update(4, 5);

    cout << "Updated array:" << endl;
    msf.printArray();

    max_sum = msf.maxSubarraySum();
    indices = msf.maxSubarrayIndices();
    start = indices.first;
    end = indices.second;

    cout << "New maximum subarray sum: " << max_sum << endl;
    cout << "New maximum subarray: [" << start << ", " << end << "]" << endl;

    return 0;
}
