#include <iostream>
using namespace std;

struct Node {
    int val;
    Node* left;
    Node* right;
    Node(int x) : val(x), left(nullptr), right(nullptr) {}
};

int dfs(Node* node, int sum) {
    if (!node) return 0;

    sum += node->val;

    if (!node->left && !node->right) {
        return sum;
    }

    return dfs(node->left, sum) + dfs(node->right, sum);
}

int main() {
    Node* root = new Node(5);
    root->left = new Node(4);
    root->right = new Node(8);
    root->left->left = new Node(11);
    root->right->left = new Node(13);
    root->right->right = new Node(4);
    root->left->left->left = new Node(7);
    root->left->left->right = new Node(2);
    root->right->right->left = new Node(5);
    root->right->right->right = new Node(1);

    int sum = dfs(root, 0);
    cout << "Total sum of all paths: " << sum << endl;
    return 0;
}

