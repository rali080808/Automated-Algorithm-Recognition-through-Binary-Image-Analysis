//My code :)
#include <bits/stdc++.h>
#include <vector>
using namespace std;
vector<vector<pair<int, int>>> edges;
vector<bool>used;
stack<int> ans;
int n, m;
void input();
void dfs(int s);
void print();
int main( )
{
    input();
    int brOdd=0, start=1;
    for (int i=1; i<=n; i++)
    {
        if ( edges[i].size() & 1 )
        {
            brOdd++;
            start=i;
        }
    }

    if (brOdd!=0 && brOdd!=2)
    {
        cout << "The graph is not Euler!\n";
        return 0;
    }

    dfs(start);
    print();
}
void print()
{
    while (!ans.empty())
    {
        cout << ans.top() << endl;
        ans.pop();
    }
}
void dfs(int s)
{
    for (int i=0; i<edges[s].size(); i++)
    {
        int v = edges[s][i].first;
        int nom = edges[s][i].second;
        if ( !used[nom] )
        {
            used[nom]=1;
            dfs(v);
        }
    }
    ans.push(s);
}
void input()
{

    cin >> n >> m;
    edges.resize(n+1);
    used.resize(m);
    int v1, v2;
    for (int nom=0; nom<m; nom++)
    {
        cin >> v1 >> v2;

        edges[v1].push_back({v2, nom});
        edges[v2].push_back({v1, nom});
    }
}
/***
4 5
1 2
1 3
1 4
2 3
3 4
*/
/**
6 8
1 2
1 5
1 6
1 4
4 3
3 2
2 5
2 6*/

