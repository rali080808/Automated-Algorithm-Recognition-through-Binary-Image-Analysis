#include <bits/stdc++.h>
#define MAXN 1008
using namespace std;
vector<int> edges[MAXN];
bool ap[MAXN];
bool used[MAXN];
int up[MAXN];
int disc[MAXN];
int t = 0;
int ch[MAXN];
void dfsAP(int s, int p)
{
    used[s] = true;
    up[s] = disc[s] = ++t;
    for (auto next : edges[s])
    {
        if ( !used[next] )
        {
            ++ch[s];
            dfsAP(next, s);
            up[s] = min(up[s], up[next]); //after dfs
            if ( p != -1 && disc[s] <= up[next] )
                ap[s] = true;
        }
        else up[s] = min(up[s], disc[next]); // we've come to an already visited node using a back edge
    }
    if ( p == -1 && ch[s] >= 2 )
        ap[s] = true;
}
int n;
void findArticulationPoints()
{
    for (int i=1; i<=n; i++)
    {
        if ( !used[i] )
            dfsAP(i, -1);
    }
}
int main()
{
    int m;
    cin >> n >> m;
    for (int j=0; j<m; j++)
    {
        int b, e;
        cin >> b >> e;
        edges[b].push_back(e);
    }
    findArticulationPoints();

    cout << "ArticulationPoints: ";
    for (int i=1; i<=n; i++)
    {
        if ( ap[i] ) cout << i << ", ";
    }
}
/**
6 7
1 2
2 3
3 1
2 4
4 5
5 6
6 4
---------------------
ArticulationPoints: 2, 4,
*/

