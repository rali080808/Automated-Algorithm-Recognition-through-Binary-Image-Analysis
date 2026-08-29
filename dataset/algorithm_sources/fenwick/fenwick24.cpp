#include <iostream>
#include <vector>
#include <complex>
using namespace std;

// Fenwick Tree implementation for complex numbers
// Can be used for frequency domain operations and signal processing
class ComplexFenwick {
private:
    vector<complex<double>> tree;
    int n;

public:
    ComplexFenwick(int size) {
        n = size;
        tree.assign(n + 1, complex<double>(0, 0));
    }

    // Update position idx with complex value val
    void update(int idx, const complex<double>& val) {
        while (idx <= n) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    // Get complex sum in range [1...idx]
    complex<double> query(int idx) {
        complex<double> sum(0, 0);
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }
        return sum;
    }

    // Get complex sum in range [left...right]
    complex<double> rangeQuery(int left, int right) {
        return query(right) - query(left - 1);
    }

    // Apply a phase shift to a specific position
    void phaseShift(int idx, double angle) {
        complex<double> factor(cos(angle), sin(angle));
        complex<double> oldVal = rangeQuery(idx, idx);
        complex<double> newVal = oldVal * factor;

        // Remove old value
        update(idx, -oldVal);
        // Add new value
        update(idx, newVal);
    }

    // Scale amplitude at a position
    void scaleAmplitude(int idx, double scale) {
        complex<double> oldVal = rangeQuery(idx, idx);
        complex<double> newVal = oldVal * scale;

        // Remove old value
        update(idx, -oldVal);
        // Add new value
        update(idx, newVal);
    }

    // Print the complex value at each position
    void printValues() {
        cout << "Position values:" << endl;
        for (int i = 1; i <= n; i++) {
            complex<double> val = rangeQuery(i, i);
            cout << "Position " << i << ": " << val.real() << " + " << val.imag() << "i";
            cout << " (magnitude: " << abs(val) << ", phase: " << arg(val) << ")" << endl;
        }
    }
};

int main() {
    // Create a complex Fenwick tree with 8 positions
    ComplexFenwick tree(8);

    // Add some complex values
    tree.update(1, complex<double>(1.0, 0.5));
    tree.update(3, complex<double>(2.0, 1.0));
    tree.update(5, complex<double>(0.5, 2.0));
    tree.update(7, complex<double>(1.5, 1.5));

    cout << "Initial values:" << endl;
    tree.printValues();

    // Get the sum of a range
    complex<double> sum = tree.rangeQuery(2, 6);
    cout << "\nSum of range [2,6]: " << sum.real() << " + " << sum.imag() << "i" << endl;

    // Apply a 90-degree phase shift to position 3
    cout << "\nApplying 90-degree phase shift to position 3" << endl;
    tree.phaseShift(3, 3.14159265 / 2); // pi/2 radians = 90 degrees

    // Apply amplitude scaling to position 5
    cout << "Scaling amplitude at position 5 by factor of 2" << endl;
    tree.scaleAmplitude(5, 2.0);

    cout << "\nAfter phase shift and scaling:" << endl;
    tree.printValues();

    // Calculate a new range sum
    sum = tree.rangeQuery(2, 6);
    cout << "\nNew sum of range [2,6]: " << sum.real() << " + " << sum.imag() << "i" << endl;
    cout << "With magnitude: " << abs(sum) << " and phase: " << arg(sum) << " radians" << endl;

    return 0;
}
