//https://www.youtube.com/watch?v=uSFzHCZ4E-8&list=LL&index=4
//https://www.youtube.com/watch?v=RgITNht_f4Q&list=LL&index=3
#include <bits/stdc++.h>
#define MAXN 1001
using namespace std;
int a[MAXN];
int n;
int lsb(int i) // least significant bit
{
    return i & -i;
}
void build()
{
    for (int i=1; i<=n; i++)
    {
        int p = i + lsb(i);
        if ( p <= n ) a[p] += a[i];
    }
}
void add(int i, int s)
{
    while ( i <= n )
    {
        a[i] += s;
        i += lsb(i);
    }
}
int sum(int i)
{
    int ret = 0;
    while ( i > 0 )
    {
        ret += a[i];
        i -= lsb(i);
    }
    return ret;
}
int query(int L, int R)
{
    return sum(R) - sum(L-1);
}
int main()
{
    cin >> n;
    for (int i=1; i<=n; i++)
        cin >> a[i];

    build();
    for (int i=1; i<=n; i++)
        cerr << a[i] << " ";
    cerr << endl;

    int ind, s; cin >> ind >> s;
    add(ind, s);
    for (int i=1; i<=n; i++)
        cerr << a[i] << " ";
    cerr << endl;

    int L, R; cin >> L >> R;
    cout << query(L, R) << endl;
}
/*
15
5 2 9 -3 5 20 10 -7 2 3 -4 0 -2 15 5
4 10
11 15
------------------------------
5 7 9 13 5 25 10 41 2 5 -4 1 -2 13 5
5 7 9 23 5 25 10 51 2 5 -4 1 -2 13 5
14
*/

