//O(V+E)
//https://www.geeksforgeeks.org/problems/strongly-connected-components-kosarajus-algo/1
//https://www.youtube.com/watch?v=Rs6DXyWpWrI&t=1212s
/* A directed graph is called strongly connected
   if there is a path in each direction between
   each pair of vertices of the graph */
#include <bits/stdc++.h>
#define MAXN 1000
using namespace std;
vector<int> edges[MAXN];

bool used[MAXN];
stack<int>topoOrder;
void topologicalSort(int s)
{
    used[s] = true;
    for (auto v : edges[s])
    {
        if ( !used[v] )
        {
            topologicalSort(v);
        }
    }
    topoOrder.push(s);
}
vector<int> reversed[MAXN];
int component[MAXN];
void dfsReversed(int s, int components)
{
    component[s] = components;
    for (auto v : reversed[s])
    {
        if ( !component[v] )
        {
            dfsReversed(v, components);
        }
    }
}
int findComponents(int n)
{
    //topological sort
    for (int i=1; i<=n;i++)
        if ( !used[i] )
            topologicalSort(i);

    //reverse the edges
    for (int i=1; i<=n; i++)
        for (auto v : edges[i])
            reversed[v].push_back(i);

    //DFS in topological order but with reversed edges
    int components = 0;
    while ( !topoOrder.empty() )
    {
        int v = topoOrder.top();
        if ( !component[v] )
        {
            ++components;
            dfsReversed(v, components);
        }
        topoOrder.pop();
    }
    return components;
}
int main()
{
    int n, m; cin >> n >> m;
    for (int j=0; j<m; j++)
    {
        int b, e; cin >> b >> e;
        edges[b].push_back(e);
    }

    int components = findComponents(n);
    for (int comp = 1; comp <= components; comp++)
    {
        cout << "component " << comp << ":";
        for (int i=1; i<=n; i++)
            if ( component[i] == comp )
                cout << " " << i;
        cout << endl;
    }
}
/*
5 5
2 1
1 3
3 4
4 3
4 5
------------------
component 1: 2
component 2: 1
component 3: 3 4
component 4: 5
*/

