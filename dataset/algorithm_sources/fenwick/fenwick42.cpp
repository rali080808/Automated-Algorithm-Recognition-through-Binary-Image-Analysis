#include <bits/stdc++.h>
#define MAXNM 100
using namespace std;
int a[MAXNM][MAXNM];
int f[MAXNM][MAXNM];
int n, m;
void update(int r, int c, int val)
{
    while ( r <= n )
    {
        int c1 = c;
        while ( c1 <= m )
        {
            f[r][c1] += val;
            c1 += c1 & -c1;
        }
        r += r & -r;
    }
}
int sum(int r, int c)
{
    int res = 0;

    while (r > 0 )
    {
        int c1 = c;
        while ( c1 > 0 )
        {
            res += f[r][c1];
            c1 -= c1 & -c1;
        }
        r -= r & -r;
    }
    return res;
}
int query(int r1, int c1, int r2, int c2)
{
    int v1 = sum( r2, c1-1 );
    int v2 = sum( r1-1, c2);
    int v3 = sum( r1-1, c1-1 );
    int v4 = sum(r2, c2);

    return v4 - v1 - v2 + v3;
}

int main()
{
     cin >> n >> m;
    for (int i=1; i<=n; i++)
        for (int j=1; j<=m; j++)
        {
            cin >> a[i][j];
            update(i, j, a[i][j]);
        }


    cout << query(1, 1, 2, 3) << endl;;
    cout << query(1, 1, 1, 1) << endl;
    cout << query(2, 2, 2, 2) << endl;
    cout << query(2, 2, 3, 2) << endl;
    cout << query(2, 3, 4, 5) << endl;
}
/*
4 5
1     8 23  7  5
98    3 11  2  21
2838 23 22  9  19
473 238 11 88   1
------------
144
1
3
26
184
*/

