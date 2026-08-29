#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

// Compressed Fenwick Tree for handling very large coordinate ranges
// using coordinate compression
class CompressedFenwickTree {
private:
    vector<int> tree;
    vector<int> coordinates;
    map<int, int> coordToIndex;
    int n;

    // Build the coordinate mapping
    void buildCoordinateMapping() {
        // Remove duplicates and sort
        sort(coordinates.begin(), coordinates.end());
        coordinates.erase(unique(coordinates.begin(), coordinates.end()), coordinates.end());

        // Create mapping from original coordinate to compressed index
        for (int i = 0; i < coordinates.size(); i++) {
            coordToIndex[coordinates[i]] = i + 1;
        }

        // Set size and initialize tree
        n = coordinates.size();
        tree.assign(n + 1, 0);
    }

    // Convert original coordinate to compressed index
    int getCompressedIndex(int coord) {
        if (coordToIndex.find(coord) == coordToIndex.end()) {
            // If coordinate doesn't exist, return 0 (sentinel value)
            return 0;
        }
        return coordToIndex[coord];
    }

public:
    // Constructor with list of coordinates
    CompressedFenwickTree(const vector<int>& coords) : coordinates(coords) {
        buildCoordinateMapping();
    }

    // Add coordinates after initialization
    void addCoordinates(const vector<int>& coords) {
        // Add new coordinates
        coordinates.insert(coordinates.end(), coords.begin(), coords.end());

        // Rebuild mapping
        buildCoordinateMapping();
    }

    // Update value at a given coordinate
    void update(int coord, int val) {
        int idx = getCompressedIndex(coord);
        if (idx == 0) return; // Coordinate doesn't exist

        while (idx <= n) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    // Query sum up to a given coordinate
    int query(int coord) {
        int idx = getCompressedIndex(coord);
        int sum = 0;

        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }

        return sum;
    }

    // Query sum in a range [from, to]
    int rangeQuery(int from, int to) {
        return query(to) - query(from - 1);
    }

    // Get size of compressed tree
    int size() const {
        return n;
    }

    // Get original coordinate from compressed index
    int getOriginalCoordinate(int idx) {
        if (idx <= 0 || idx > n) return -1;
        return coordinates[idx - 1];
    }

    // Print mapping information
    void printMapping() {
        cout << "Coordinate Compression Mapping:" << endl;
        cout << "Original -> Compressed" << endl;
        cout << "---------------------" << endl;

        for (const auto& pair : coordToIndex) {
            cout << pair.first << " -> " << pair.second << endl;
        }
    }

    // Print tree information
    void printTree() {
        cout << "Compressed Fenwick Tree:" << endl;
        for (int i = 1; i <= n; i++) {
            cout << "Index " << i << " (Coord " << getOriginalCoordinate(i)
                 << "): " << tree[i] << endl;
        }
    }
};

// Example application: Range counting with very large coordinates
void rangeCountingExample() {
    // Very large coordinates (imagine these as points on a 2D space compressed to 1D)
    vector<int> points = {1000000, 2000000, 5000000, 8000000, 10000000, 15000000};

    // Create compressed Fenwick tree with these coordinates
    CompressedFenwickTree tree(points);

    // Add a point at each coordinate
    for (int point : points) {
        tree.update(point, 1);
    }

    // Print compression information
    cout << "Original space requires array of size 15,000,000" << endl;
    cout << "Compressed space requires array of size " << tree.size() << endl;
    tree.printMapping();

    // Count points in different ranges
    cout << "\nPoints count in range [1000000, 5000000]: "
         << tree.rangeQuery(1000000, 5000000) << endl;

    cout << "Points count in range [2000000, 10000000]: "
         << tree.rangeQuery(2000000, 10000000) << endl;

    // Add more points
    cout << "\nAdding more points at coordinates 3000000 and 12000000..." << endl;
    tree.addCoordinates({3000000, 12000000});
    tree.update(3000000, 1);
    tree.update(12000000, 1);

    // Print updated mapping
    tree.printMapping();

    // Count points in same ranges after update
    cout << "\nPoints count in range [1000000, 5000000] after update: "
         << tree.rangeQuery(1000000, 5000000) << endl;

    cout << "Points count in range [2000000, 12000000] after update: "
         << tree.rangeQuery(2000000, 12000000) << endl;
}

int main() {
    cout << "=== Compressed Fenwick Tree Example ===" << endl;

    // Example coordinates (could be very large values in real applications)
    vector<int> coords = {100, 500, 1000, 1500, 3000, 5000, 8000};

    // Create compressed Fenwick tree
    CompressedFenwickTree tree(coords);

    // Add values at some coordinates
    tree.update(100, 10);
    tree.update(1000, 25);
    tree.update(3000, 15);
    tree.update(8000, 20);

    // Print compression mapping
    tree.printMapping();

    // Print tree values
    tree.printTree();

    // Query some ranges
    cout << "\nRange Queries:" << endl;
    cout << "Sum in range [100, 1000]: " << tree.rangeQuery(100, 1000) << endl;
    cout << "Sum in range [1000, 5000]: " << tree.rangeQuery(1000, 5000) << endl;
    cout << "Sum in range [500, 8000]: " << tree.rangeQuery(500, 8000) << endl;

    // Add more coordinates
    cout << "\nAdding new coordinates: 2000, 4000, 9000" << endl;
    tree.addCoordinates({2000, 4000, 9000});

    // Update values at new coordinates
    tree.update(2000, 12);
    tree.update(9000, 18);

    // Print updated mapping
    tree.printMapping();

    // Query again
    cout << "\nUpdated Range Queries:" << endl;
    cout << "Sum in range [1000, 5000]: " << tree.rangeQuery(1000, 5000) << endl;
    cout << "Sum in range [2000, 9000]: " << tree.rangeQuery(2000, 9000) << endl;

    // Demonstrate application with very large coordinates
    cout << "\n=== Application with Very Large Coordinates ===" << endl;
    rangeCountingExample();

    return 0;
}
