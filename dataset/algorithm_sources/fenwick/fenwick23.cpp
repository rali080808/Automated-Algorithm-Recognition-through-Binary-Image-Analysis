#include <iostream>
#include <vector>
#include <memory>
using namespace std;

// Persistent Fenwick Tree implementation
// This allows querying historical versions of the tree
class PersistentFenwick {
private:
    struct Node {
        int value;
        shared_ptr<Node> left, right;

        Node(int val = 0) : value(val), left(nullptr), right(nullptr) {}
    };

    vector<shared_ptr<Node>> roots; // Store root for each version
    int n;

    // Helper function to get sum up to pos in a specific version
    int queryUtil(shared_ptr<Node> node, int l, int r, int pos) {
        if (!node) return 0;
        if (l == r) return node->value;

        int mid = (l + r) / 2;
        if (pos <= mid) {
            return queryUtil(node->left, l, mid, pos);
        } else {
            return node->value - queryUtil(node->right, mid + 1, r, pos);
        }
    }

    // Helper function to update the value at idx for a new version
    shared_ptr<Node> updateUtil(shared_ptr<Node> node, int l, int r, int idx, int val) {
        if (!node) node = make_shared<Node>();

        if (l == r) {
            shared_ptr<Node> newNode = make_shared<Node>(node->value + val);
            return newNode;
        }

        int mid = (l + r) / 2;
        shared_ptr<Node> newNode = make_shared<Node>(*node);

        if (idx <= mid) {
            newNode->left = updateUtil(node->left, l, mid, idx, val);
        } else {
            newNode->right = updateUtil(node->right, mid + 1, r, idx, val);
        }

        // Update the value of the new node
        newNode->value = (newNode->left ? newNode->left->value : 0) +
                         (newNode->right ? newNode->right->value : 0);

        return newNode;
    }

public:
    PersistentFenwick(int size) {
        n = size;
        // Create version 0 (empty tree)
        roots.push_back(nullptr);
    }

    // Create a new version with an update at idx
    void update(int idx, int val) {
        if (idx <= 0 || idx > n) return;

        // Get the latest version
        shared_ptr<Node> latestRoot = roots.back();

        // Create a new version with the update
        shared_ptr<Node> newRoot = updateUtil(latestRoot, 1, n, idx, val);
        roots.push_back(newRoot);
    }

    // Query the sum in range [1, pos] for a specific version
    int query(int version, int pos) {
        if (version < 0 || version >= roots.size() || pos <= 0 || pos > n) {
            return 0;
        }

        return queryUtil(roots[version], 1, n, pos);
    }

    // Query the sum in range [left, right] for a specific version
    int rangeQuery(int version, int left, int right) {
        if (left > right) return 0;
        return query(version, right) - query(version, left - 1);
    }

    // Get the number of versions
    int versionCount() {
        return roots.size();
    }
};

int main() {
    PersistentFenwick tree(10);

    // Create version 1 with update at index 3
    tree.update(3, 5);

    // Create version 2 with update at index 5
    tree.update(5, 7);

    // Create version 3 with update at index 3 again
    tree.update(3, 2);

    // Create version 4 with update at index 8
    tree.update(8, 9);

    // Query different versions
    cout << "Version 1, Sum[1,5]: " << tree.rangeQuery(1, 1, 5) << endl;
    cout << "Version 2, Sum[1,5]: " << tree.rangeQuery(2, 1, 5) << endl;
    cout << "Version 3, Sum[1,5]: " << tree.rangeQuery(3, 1, 5) << endl;
    cout << "Version 4, Sum[1,10]: " << tree.rangeQuery(4, 1, 10) << endl;

    // Compare different versions for the same range
    cout << "\nTracking changes to Sum[1,8] across versions:" << endl;
    for (int v = 1; v < tree.versionCount(); v++) {
        cout << "Version " << v << ": " << tree.rangeQuery(v, 1, 8) << endl;
    }

    return 0;
}
