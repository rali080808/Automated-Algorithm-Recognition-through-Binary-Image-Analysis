#include <bits/stdc++.h>
#define endl '\n'
#define MAXN 1008
using namespace std;
bool r[MAXN];
stack<int>topo;
vector<int> edges[MAXN];
bool used[MAXN];
void dfs(int s)
{
   /// cerr << s << endl;
    used[s] = true;
    for (auto v : edges[s])
    {
        if ( !used[v] )
            dfs(v);
    }
  ///  cerr << "push " << s << endl;
    topo.push(s);
}
int main()
{
    int n, m; cin >> n >> m;
    memset(r, 1, sizeof(r));
    for (int j=0; j<m; j++)
    {
        int a, b, c; cin >> a >> b;
        edges[a].push_back(b);
        r[b] = false;
    }
    queue<int> roots;
    for (int i=1; i<=n; i++)
    {
        if ( r[i] ) roots.push(i);
    }
    if ( !roots.size() ) return 0;


    while ( !roots.empty() )
    {
        cerr << "root " << roots.front() << endl;
        dfs(roots.front());
        roots.pop();
        while ( !topo.empty() )
        {
            cout << topo.top() << " ";
            topo.pop();
        }

    }
    cout << endl;
}
/**
5 5
1 3
5 3
1 5
1 2
1 4

===========
1 2 4 3 5
1 2 4 5 3
*/


