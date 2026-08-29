#include <iostream>
#include <vector>
using namespace std;

class Fenwick2D {
    vector<vector<int>> bit;
    int n, m;
public:
    Fenwick2D(int rows, int cols) : n(rows), m(cols), bit(rows + 1, vector<int>(cols + 1)) {}

    void update(int x, int y, int delta) {
        for (int i = x; i <= n; i += i & -i)
            for (int j = y; j <= m; j += j & -j)
                bit[i][j] += delta;
    }

    int query(int x, int y) {
        int res = 0;
        for (int i = x; i > 0; i -= i & -i)
            for (int j = y; j > 0; j -= j & -j)
                res += bit[i][j];
        return res;
    }

    int rangeSum(int x1, int y1, int x2, int y2) {
        return query(x2, y2) - query(x1 - 1, y2) - query(x2, y1 - 1) + query(x1 - 1, y1 - 1);
    }
};

int main() {
    Fenwick2D ft(5, 5);
    ft.update(2, 3, 5);
    ft.update(4, 4, 7);
    cout << ft.rangeSum(2, 3, 4, 4) << endl;  // Output: 12
}
