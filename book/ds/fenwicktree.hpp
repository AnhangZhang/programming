#include "book/alg/monoid/add.hpp"

template <typename Monoid>
struct FenwickTree {
  using G = Monoid;
  using T = typename G::value_type;
  static_assert(G::commute);
  int n;
  vt<T> dat;
  T total;

  FenwickTree() {}
  FenwickTree(int n) { build(n); }
  template <typename F>
  FenwickTree(int n, F f) {
    build(n, f);
  }
  FenwickTree(const vt<T>& v) { build(v); }

  void build(int m) {
    n = m;
    dat.assign(n, G::e());
    total = G::e();
  }
  void build(const vt<T>& v) {
    build(si(v), [&](int i) -> T { return v[i]; });
  }
  template <typename F>
  void build(int m, F f) {
    n = m;
    total = G::e();
    dat.assign(n, G::e());
    FOR(i, 1, n + 1) {
      dat[i - 1] = G::op(dat[i - 1], f(i - 1));
      int j = i + (i & -i);
      if (j <= n) dat[j - 1] = G::op(dat[j - 1], dat[i - 1]);
    }
    total = prefix_prod(n);
  }
  T sum_all() const { return total; }
  T prefix_prod(int i) const {
    ckmin(i, n);
    T ret = G::e();
    for (; i > 0; i -= i & -i) ret = G::op(ret, dat[i - 1]);
    return ret;
  }
  T prod(int i) const { return prefix_prod(i); }
  T prod(int l, int r) const {
    ckmax(l, 0);
    ckmin(r, n);
    if (l == 0) return prefix_prod(r);
    assert(0 <= l && l <= r && r <= n);
    T pos = G::e(), neg = G::e();
    while (l < r) {
      pos = G::op(pos, dat[r - 1]), r -= r & -r;
    }
    while (r < l) {
      neg = G::op(neg, dat[l - 1]), l -= l & -l;
    }
    return G::op(pos, G::inverse(neg));
  }
  void add(int i, T x) {
    assert(0 <= i && i < n);
    total = G::op(total, x);
    for (++i; i <= n; i += i & -i) dat[i - 1] = G::op(dat[i - 1], x);
  }
  void set(int i, T x) { add(i, G::op(G::inverse(prod(i, i + 1)), x)); }
};