#include <iostream>
#include <vector>
using namespace std;

class FenwickTree2D {
private:
    vector<vector<int>> tree;
    int n, m;

public:
    FenwickTree2D(int rows, int cols) {
        n = rows;
        m = cols;
        tree.assign(n + 1, vector<int>(m + 1, 0));
    }

    // Update point (x, y) with value val
    void update(int x, int y, int val) {
        for (int i = x; i <= n; i += i & -i) {
            for (int j = y; j <= m; j += j & -j) {
                tree[i][j] += val;
            }
        }
    }

    // Get sum of rectangle from (1,1) to (x,y)
    int query(int x, int y) {
        int sum = 0;
        for (int i = x; i > 0; i -= i & -i) {
            for (int j = y; j > 0; j -= j & -j) {
                sum += tree[i][j];
            }
        }
        return sum;
    }

    // Get sum of rectangle from (x1,y1) to (x2,y2)
    int rangeQuery(int x1, int y1, int x2, int y2) {
        return query(x2, y2) - query(x2, y1 - 1) - query(x1 - 1, y2) + query(x1 - 1, y1 - 1);
    }
};

int main() {
    // 5x5 grid
    FenwickTree2D bit2d(5, 5);

    // Add values to some positions
    bit2d.update(1, 2, 3);
    bit2d.update(2, 3, 4);
    bit2d.update(3, 1, 2);
    bit2d.update(4, 4, 5);

    // Query some ranges
    cout << "Sum of rectangle (1,1) to (3,3): " << bit2d.rangeQuery(1, 1, 3, 3) << endl;
    cout << "Sum of rectangle (2,2) to (4,4): " << bit2d.rangeQuery(2, 2, 4, 4) << endl;

    return 0;
}
