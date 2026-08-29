#include <iostream>
#include <vector>
using namespace std;

// 3D Fenwick Tree implementation for 3D range queries and updates
class FenwickTree3D {
private:
    vector<vector<vector<int>>> tree;
    int n, m, p; // dimensions: n x m x p

public:
    FenwickTree3D(int n_dim, int m_dim, int p_dim) {
        n = n_dim;
        m = m_dim;
        p = p_dim;
        tree.assign(n + 1, vector<vector<int>>(m + 1, vector<int>(p + 1, 0)));
    }

    // Update point (x, y, z) with value val
    void update(int x, int y, int z, int val) {
        for (int i = x; i <= n; i += i & -i) {
            for (int j = y; j <= m; j += j & -j) {
                for (int k = z; k <= p; k += k & -k) {
                    tree[i][j][k] += val;
                }
            }
        }
    }

    // Get sum in 3D rectangle from (1,1,1) to (x,y,z)
    int query(int x, int y, int z) {
        int sum = 0;
        for (int i = x; i > 0; i -= i & -i) {
            for (int j = y; j > 0; j -= j & -j) {
                for (int k = z; k > 0; k -= k & -k) {
                    sum += tree[i][j][k];
                }
            }
        }
        return sum;
    }

    // Calculate sum in 3D rectangle from (x1,y1,z1) to (x2,y2,z2)
    int rangeQuery(int x1, int y1, int z1, int x2, int y2, int z2) {
        // Ensure proper order
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        if (z1 > z2) swap(z1, z2);

        // Calculate using inclusion-exclusion principle
        return query(x2, y2, z2) - query(x2, y2, z1-1) - query(x2, y1-1, z2) - query(x1-1, y2, z2)
             + query(x1-1, y1-1, z2) + query(x1-1, y2, z1-1) + query(x2, y1-1, z1-1)
             - query(x1-1, y1-1, z1-1);
    }

    // Set the value at a specific 3D point (equivalent to update with delta)
    void set(int x, int y, int z, int val) {
        int current = rangeQuery(x, y, z, x, y, z);
        update(x, y, z, val - current);
    }

    // Print the sum of each layer
    void printLayerSums() {
        cout << "Layer sums:" << endl;
        for (int z = 1; z <= p; z++) {
            cout << "Layer " << z << ": " << rangeQuery(1, 1, z, n, m, z) << endl;
        }
    }
};

int main() {
    // Create a 4x4x4 3D Fenwick Tree
    FenwickTree3D cube(4, 4, 4);

    // Add some values
    cube.update(1, 2, 3, 5);  // Value 5 at point (1,2,3)
    cube.update(2, 3, 1, 3);  // Value 3 at point (2,3,1)
    cube.update(4, 4, 4, 7);  // Value 7 at point (4,4,4)
    cube.update(3, 1, 2, 2);  // Value 2 at point (3,1,2)

    // Query some 3D ranges
    cout << "Sum in cube (1,1,1) to (2,2,2): " << cube.rangeQuery(1, 1, 1, 2, 2, 2) << endl;
    cout << "Sum in cube (2,2,2) to (4,4,4): " << cube.rangeQuery(2, 2, 2, 4, 4, 4) << endl;
    cout << "Sum in entire 4x4x4 cube: " << cube.rangeQuery(1, 1, 1, 4, 4, 4) << endl;

    // Print layer-by-layer sums
    cube.printLayerSums();

    // Set a value at a specific point
    cout << "\nSetting value at (2,2,2) to 10..." << endl;
    cube.set(2, 2, 2, 10);

    // Query again after the update
    cout << "Sum in cube (1,1,1) to (2,2,2) after update: " << cube.rangeQuery(1, 1, 1, 2, 2, 2) << endl;
    cout << "Sum in entire 4x4x4 cube after update: " << cube.rangeQuery(1, 1, 1, 4, 4, 4) << endl;

    // Example of a real-world application: 3D radiation dose calculation
    cout << "\nExample: 3D Radiation Dose Calculation" << endl;
    FenwickTree3D doseCube(10, 10, 10);

    // Simulate radiation sources
    doseCube.update(3, 3, 3, 100);  // Strong radiation source at (3,3,3)
    doseCube.update(7, 7, 7, 80);   // Another radiation source at (7,7,7)

    // Calculate dose in different volumes
    cout << "Dose in central region (4,4,4) to (6,6,6): " << doseCube.rangeQuery(4, 4, 4, 6, 6, 6) << endl;
    cout << "Dose in upper quadrant (1,1,1) to (5,5,5): " << doseCube.rangeQuery(1, 1, 1, 5, 5, 5) << endl;
    cout << "Dose in lower quadrant (6,6,6) to (10,10,10): " << doseCube.rangeQuery(6, 6, 6, 10, 10, 10) << endl;

    return 0;
}
