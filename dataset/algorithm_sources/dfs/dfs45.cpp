#include <bits/stdc++.h>
#define MAXN 802
#define MAXM 1002
using namespace std;
vector<int> edges[MAXN];
bool used[MAXN];
void dfs(int s)
{
    stack<int> st;
    st.push(s);
    used[s] = true;
    cerr << s;
    while (!st.empty())
    {

        s = st.top();

        bool found = false;
        int sz = edges[s].size();
        for (int i=0; i<sz; i++)
        {
            if (!used[edges[s][i]])
            {cerr << " " << edges[s][i];
                used[edges[s][i]] = true;
                st.push(edges[s][i]);
                found = true;
                break;
            }
        }
        if (!found) st.pop();
    }
}
/*
10 13
1 3
1 4
1 5
1 8
1 9
2 5
2 9
3 4
3 6
4 6
5 8
6 7
6 10

*/
int main( )
{
    int n;
    int m;
    int s; //start

    cin >> n >> m;// >> s;
    for (int j=0; j<m; j++)
    {
        int b, e;
        cin >> b >> e;
        edges[b].push_back(e);
        edges[e].push_back(b);
    }

    dfs(4);
}

