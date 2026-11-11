#pragma once

#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using i128 = __int128;
using u128 = unsigned __int128;
using f128 = __float128;
using db = double;
using str = string;
using pi = pair<ll, ll>;

template <class T>
constexpr T infty = numeric_limits<T>::max() / 4;
template <>
constexpr int infty<int> = 1'100'000'000;
template <>
constexpr ll infty<ll> = 2'200'000'000'000'000'000;

#define all(x) (x).begin(), (x).end()
#define si(x) (long long)(x).size()
#define pb push_back
#define eb emplace_back
#define mp make_pair
#define mt make_tuple
#define fi first
#define se second

template <class T>
using vt = vector<T>;
template <class T>
using vvt = vector<vector<T>>;
using vi = vt<ll>;
using vpi = vt<pi>;
using vs = vt<str>;

#define vv(type, name, h, ...) \
  vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define vvv(type, name, h, w, ...)   \
  vector<vector<vector<type>>> name( \
      h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))

#define FOR1(a) for (ll _ = 0; _ < (ll)a; _++)
#define FOR2(i, a) for (ll i = 0; i < (ll)a; i++)
#define FOR3(i, a, b) for (ll i = a; i < (ll)b; i++)
#define FOR1_R(a) for (ll _ = (ll)a - 1; _ >= 0; _--)
#define FOR2_R(i, a) for (ll i = (ll)a - 1; i >= 0; i--)
#define FOR3_R(i, a, b) for (ll i = (ll)b - 1; i >= (ll)a; i--)
#define overload3(a, b, c, d, ...) d
#define FOR(...) overload3(__VA_ARGS__, FOR3, FOR2, FOR1)(__VA_ARGS__)
#define FOR_R(...) overload3(__VA_ARGS__, FOR3_R, FOR2_R, FOR1_R)(__VA_ARGS__)

template <class T>
using min_pq = priority_queue<T, vector<T>, greater<T>>;
template <class T>
using max_pq = priority_queue<T>;

template <class T, class U>
bool ckmin(T& a, const U& b) {
  T c = min<T>(a, b);
  bool changed = (c != a);
  a = c;
  return changed;
}
template <class T, class U>
bool ckmax(T& a, const U& b) {
  T c = max<T>(a, b);
  bool changed = (c != a);
  a = c;
  return changed;
}

template <typename T>
T floor(T x, T y) {
  return x / y - (x % y < 0);
}
template <typename T>
T ceil(T x, T y) {
  return x / y + (x % y > 0);
}

template <class T, class Container, class Compare>
T POP(priority_queue<T, Container, Compare>& que) {
  T a = que.top();
  que.pop();
  return a;
}
template <class T>
T POP(queue<T>& que) {
  T a = que.front();
  que.pop();
  return a;
}

template <typename T, typename U>
T SUM(const U& A) {
  return std::accumulate(all(A), T{});
}

#define MIN(v) *min_element(all(v))
#define MAX(v) *max_element(all(v))
#define UNIQUE(v) sort(all(v)), v.erase(unique(all(v)), v.end())

template <typename T, typename U>
vt<T> cumsum(const vt<U>& A, int off = 1) {
  int N = si(A);
  vt<T> pref(N + 1);
  FOR(i, N) pref[i + 1] = pref[i] + A[i];
  if (off == 0) pref.erase(pref.begin());
  return pref;
}