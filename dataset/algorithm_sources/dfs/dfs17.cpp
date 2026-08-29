#include <iostream>
#include <vector>
using namespace std;

void dfs(int idx, const vector<int>& set, vector<int>& current, vector<vector<int>>& subsets) {
    if (idx == set.size()) {
        subsets.push_back(current);
        return;
    }

    // Include current element
    current.push_back(set[idx]);
    dfs(idx + 1, set, current, subsets);

    // Exclude current element
    current.pop_back();
    dfs(idx + 1, set, current, subsets);
}

int main() {
    vector<int> set = {1, 2, 3};
    vector<int> current;
    vector<vector<int>> subsets;

    dfs(0, set, current, subsets);

    cout << "All subsets:" << endl;
    for (const auto& subset : subsets) {
        for (int num : subset)
            cout << num << " ";
        cout << endl;
    }

    return 0;
}

