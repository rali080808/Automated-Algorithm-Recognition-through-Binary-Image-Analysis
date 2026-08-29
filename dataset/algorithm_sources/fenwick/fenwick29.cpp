#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

// Fenwick Tree for maintaining rolling statistics (sum, sum of squares)
// Allows calculation of mean, variance, and standard deviation in ranges
class RollingStatisticsFenwick {
public:
    vector<double> sumTree;      // Tree for storing sums
    vector<double> sumSqTree;    // Tree for storing sum of squares
    int n;

    // Helper to update a tree
    void updateTree(vector<double>& tree, int idx, double val) {
        while (idx <= n) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    // Helper to query a tree
    double queryTree(const vector<double>& tree, int idx) {
        double sum = 0.0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }
        return sum;
    }

public:
    RollingStatisticsFenwick(int size) : n(size) {
        sumTree.assign(n + 1, 0.0);
        sumSqTree.assign(n + 1, 0.0);
    }

    // Update the value at position idx
    void update(int idx, double val) {
        if (idx <= 0 || idx > n) return;

        // Get current value
        double currentVal = getValue(idx);

        // Calculate deltas
        double deltaSum = val - currentVal;
        double deltaSumSq = val*val - currentVal*currentVal;

        // Update trees
        updateTree(sumTree, idx, deltaSum);
        updateTree(sumSqTree, idx, deltaSumSq);
    }

    // Get value at position idx
    double getValue(int idx) {
        if (idx <= 0 || idx > n) return 0.0;
        double sumUpToIdx = queryTree(sumTree, idx);
        double sumUpToPrevIdx = queryTree(sumTree, idx - 1);
        return sumUpToIdx - sumUpToPrevIdx;
    }

    // Get sum in range [1, idx]
    double sum(int idx) {
        return queryTree(sumTree, idx);
    }

    // Get sum of squares in range [1, idx]
    double sumOfSquares(int idx) {
        return queryTree(sumSqTree, idx);
    }

    // Get sum in range [left, right]
    double rangeSum(int left, int right) {
        if (left > right) return 0.0;
        return sum(right) - sum(left - 1);
    }

    // Get sum of squares in range [left, right]
    double rangeSumOfSquares(int left, int right) {
        if (left > right) return 0.0;
        return sumOfSquares(right) - sumOfSquares(left - 1);
    }

    // Calculate mean in range [left, right]
    double rangeMean(int left, int right) {
        if (left > right || left <= 0 || right > n) return 0.0;
        int count = right - left + 1;
        return rangeSum(left, right) / count;
    }

    // Calculate variance in range [left, right]
    double rangeVariance(int left, int right) {
        if (left > right || left <= 0 || right > n) return 0.0;
        int count = right - left + 1;
        double mean = rangeMean(left, right);
        double sumSq = rangeSumOfSquares(left, right);
        return (sumSq / count) - (mean * mean);
    }

    // Calculate standard deviation in range [left, right]
    double rangeStdDev(int left, int right) {
        return sqrt(rangeVariance(left, right));
    }

    // Print statistics for a range
    void printRangeStats(int left, int right) {
        cout << "Statistics for range [" << left << ", " << right << "]:" << endl;
        cout << "  Count: " << (right - left + 1) << endl;
        cout << "  Sum: " << rangeSum(left, right) << endl;
        cout << "  Mean: " << rangeMean(left, right) << endl;
        cout << "  Variance: " << rangeVariance(left, right) << endl;
        cout << "  Standard Deviation: " << rangeStdDev(left, right) << endl;
    }

    // Print all values in the array
    void printValues() {
        cout << "Values: ";
        for (int i = 1; i <= n; i++) {
            cout << getValue(i) << " ";
        }
        cout << endl;
    }
};

// Function to demonstrate sliding window statistics
void demonstrateSlidingWindow(RollingStatisticsFenwick& stats, int windowSize) {
    cout << "\n=== Sliding Window Statistics (Window Size: " << windowSize << ") ===" << endl;
    cout << setw(10) << "Window" << setw(10) << "Mean" << setw(12) << "StdDev" << endl;
    cout << "---------------------------------------" << endl;

    for (int start = 1; start <= stats.n - windowSize + 1; start++) {
        int end = start + windowSize - 1;
        cout << setw(5) << start << "-" << setw(3) << end << "   "
             << setw(9) << fixed << setprecision(2) << stats.rangeMean(start, end) << "   "
             << setw(9) << fixed << setprecision(2) << stats.rangeStdDev(start, end) << endl;
    }
}

int main() {
    // Create a Fenwick tree for statistics with 15 positions
    RollingStatisticsFenwick stats(15);

    // Insert some test data (e.g., daily temperatures)
    vector<double> temperatures = {
        22.5, 23.1, 25.8, 28.3, 27.9,
        26.2, 24.8, 23.5, 24.1, 25.5,
        26.7, 28.9, 30.2, 29.5, 27.3
    };

    // Add data to the tree
    for (int i = 0; i < temperatures.size(); i++) {
        stats.update(i + 1, temperatures[i]);
    }

    // Print all values
    cout << "Daily Temperatures:" << endl;
    stats.printValues();

    // Print statistics for various ranges
    cout << "\n=== Statistics for Different Date Ranges ===" << endl;

    // Full range stats
    stats.printRangeStats(1, 15);

    // First week stats
    cout << "\nFirst Week:" << endl;
    stats.printRangeStats(1, 7);

    // Second week stats
    cout << "\nSecond Week:" << endl;
    stats.printRangeStats(8, 14);

    // Calculate mean daily temperature difference between weeks
    double week1Mean = stats.rangeMean(1, 7);
    double week2Mean = stats.rangeMean(8, 14);
    cout << "\nMean Temperature Difference Between Weeks: "
         << fixed << setprecision(2) << (week2Mean - week1Mean) << "°C" << endl;

    // Demonstrate sliding window statistics
    demonstrateSlidingWindow(stats, 5); // 5-day moving average

    // Update a value and recalculate statistics
    cout << "\n=== After Updating Day 7 Temperature from "
         << stats.getValue(7) << " to 29.0 ===" << endl;
    stats.update(7, 29.0);

    // Show updated statistics
    stats.printRangeStats(1, 15);
    demonstrateSlidingWindow(stats, 5);

    return 0;
}
