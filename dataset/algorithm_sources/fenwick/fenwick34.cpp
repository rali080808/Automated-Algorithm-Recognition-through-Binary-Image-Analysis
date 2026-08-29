// Example 4: 2D Fenwick Tree for 2D Range Sum Queries
#include <iostream>
#include <vector>
using namespace std;

class FenwickTree2D {
private:
    vector<vector<int>> bit;
    int rows, cols;

public:
    FenwickTree2D(int n, int m) : rows(n), cols(m) {
        bit.assign(n + 1, vector<int>(m + 1, 0));
    }

    void update(int x, int y, int val) {
        for (int i = x; i <= rows; i += i & -i)
            for (int j = y; j <= cols; j += j & -j)
                bit[i][j] += val;
    }

    int query(int x, int y) {
        int sum = 0;
        for (int i = x; i > 0; i -= i & -i)
            for (int j = y; j > 0; j -= j & -j)
                sum += bit[i][j];
        return sum;
    }

    int rangeQuery(int x1, int y1, int x2, int y2) {
        return query(x2, y2) - query(x2, y1-1) - query(x1-1, y2) + query(x1-1, y1-1);
    }
};

int main() {
    // Create a 2D grid
    vector<vector<int>> grid = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    int n = grid.size();
    int m = grid[0].size();

    FenwickTree2D ft(n, m);

    // Build the 2D Fenwick tree
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ft.update(i + 1, j + 1, grid[i][j]);
        }
    }

    // Query examples
    cout << "Sum in rectangle (1,1) to (2,2): " << ft.rangeQuery(1, 1, 2, 2) << endl;
    cout << "Sum in rectangle (2,2) to (3,3): " << ft.rangeQuery(2, 2, 3, 3) << endl;

    // Update a value and query again
    ft.update(2, 2, 5); // Add 5 to element at (2,2)
    cout << "After update, sum in rectangle (1,1) to (2,2): " << ft.rangeQuery(1, 1, 2, 2) << endl;

    return 0;
}
