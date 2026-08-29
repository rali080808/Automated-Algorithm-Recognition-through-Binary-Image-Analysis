#include <iostream>
#include <vector>
using namespace std;

// Fenwick Tree implementation for finding the kth smallest element (order statistics)
class OrderStatisticFenwick {
private:
    vector<int> bit;
    int maxVal;

public:
    // Initialize with the maximum possible value in the array
    OrderStatisticFenwick(int maxValue) {
        maxVal = maxValue;
        bit.assign(maxVal + 1, 0);
    }

    // Add element to the tree (frequency count)
    void insert(int val) {
        for (int i = val; i <= maxVal; i += i & -i) {
            bit[i]++;
        }
    }

    // Remove element from the tree
    void remove(int val) {
        for (int i = val; i <= maxVal; i += i & -i) {
            bit[i]--;
        }
    }

    // Count elements less than or equal to val
    int count(int val) {
        int sum = 0;
        for (int i = val; i > 0; i -= i & -i) {
            sum += bit[i];
        }
        return sum;
    }

    // Find kth smallest element (1-indexed)
    int findKth(int k) {
        int left = 1, right = maxVal;
        while (left < right) {
            int mid = (left + right) / 2;
            if (count(mid) >= k) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }
};

int main() {
    // Example: Find the kth smallest element in a dynamic set
    OrderStatisticFenwick tree(100); // Elements will be in range 1-100

    // Insert some elements
    vector<int> elements = {5, 2, 16, 8, 42, 23, 4, 9, 13, 20};
    for (int x : elements) {
        tree.insert(x);
    }

    // Find the 3rd smallest element
    cout << "The 3rd smallest element is: " << tree.findKth(3) << endl;

    // Find the 7th smallest element
    cout << "The 7th smallest element is: " << tree.findKth(7) << endl;

    // Remove an element
    tree.remove(5);

    // Find the 3rd smallest element after removal
    cout << "The 3rd smallest element after removing 5: " << tree.findKth(3) << endl;

    return 0;
}
