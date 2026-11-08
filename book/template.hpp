#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using db = double;
using str = string;
using pi = pair<ll, ll>;

#define all(x) (x).begin(), (x).end()
#define si(x) (long long)(x).size()
#define pb push_back
#define eb emplace_back
#define mp make_pair
#define mt make_tuple
#define f first
#define s second

template <class T>
using vt = vector<T>;
template <class T>
using vvt = vector<vector<T>>;
using vi = vt<int>;
using vpi = vt<pi>;
using vs = vt<str>;

#define vv(type, name, h, ...) \
  vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define vvv(type, name, h, w, ...)   \
  vector<vector<vector<type>>> name( \
      h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))

#define FOR1(a) for (ll i = 0; i < (ll)a; i++)
#define FOR2(i, a) for (ll i = 0; i < (ll)a; i++)
#define FOR3(i, a, b) for (ll i = a; i < (ll)b; i++)
#define FOR1_R(a) for (ll i = (ll)a - 1; i >= 0; i--)
#define FOR2_R(i, a) for (ll i = (ll)a - 1; i >= 0; i--)
#define FOR3_R(i, a, b) for (ll i = (ll)b - 1; i >= (ll)a; i--)
#define overload3(a, b, c, d, ...) d
#define FOR(...) overload3(__VA_ARGS__, FOR3, FOR2, FOR1)(__VA_ARGS__)
#define FOR_R(...) overload3(__VA_ARGS__, FOR3_R, FOR2_R, FOR1_R)(__VA_ARGS__)