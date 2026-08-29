#include <iostream>
#include <vector>
using namespace std;

template<typename T>
class FenwickTreeTemplate {
private:
    vector<T> tree;
    int n;

public:
    FenwickTreeTemplate(int size) {
        n = size;
        tree.assign(n + 1, 0);
    }

    void update(int idx, T value) {
        while (idx <= n) {
            tree[idx] += value;
            idx += idx & -idx;
        }
    }

    T query(int idx) {
        T sum = 0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }
        return sum;
    }

    T rangeQuery(int left, int right) {
        return query(right) - query(left - 1);
    }
};

int main() {
    // Fenwick tree for integers
    FenwickTreeTemplate<int> intTree(5);
    intTree.update(1, 5);
    intTree.update(2, 3);
    intTree.update(4, 7);

    cout << "Sum of range [1, 4] (int): " << intTree.rangeQuery(1, 4) << endl;

    // Fenwick tree for doubles
    FenwickTreeTemplate<double> doubleTree(5);
    doubleTree.update(1, 2.5);
    doubleTree.update(3, 1.75);
    doubleTree.update(5, 3.25);

    cout << "Sum of range [1, 5] (double): " << doubleTree.rangeQuery(1, 5) << endl;

    // Fenwick tree for long long
    FenwickTreeTemplate<long long> longTree(5);
    longTree.update(1, 1000000000LL);
    longTree.update(2, 2000000000LL);
    longTree.update(3, 3000000000LL);

    cout << "Sum of range [1, 3] (long long): " << longTree.rangeQuery(1, 3) << endl;

    return 0;
}
