#include <iostream>
#include <vector>
using namespace std;

void dfs(vector<int>& nums, vector<vector<int>>& result, int index) {
    if (index == nums.size()) {
        result.push_back(nums);
        return;
    }

    for (int i = index; i < nums.size(); i++) {
        swap(nums[index], nums[i]);
        dfs(nums, result, index + 1);
        swap(nums[index], nums[i]); // Backtrack
    }
}

int main() {
    vector<int> nums = {1, 2, 3};
    vector<vector<int>> result;

    dfs(nums, result, 0);

    cout << "Permutations:" << endl;
    for (const auto& perm : result) {
        for (int num : perm)
            cout << num << " ";
        cout << endl;
    }

    return 0;
}

