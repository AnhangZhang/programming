#include "ds/fenwicktree/fenwicktree.hpp"

struct FenwickTree_01 {
  using MX = Monoid_Add<int>;
  int N, n;
  vt<u64> dat;
  FenwickTree<MX> bit;
  FenwickTree_01() {}
  FenwickTree_01(int n) { build(n); }
  template <typename F>
  FenwickTree_01(int n, F f) {
    build(n, f);
  }

  void build(int m) {
    N = m;
    n = ceil<int>(N + 1, 64);
    dat.assign(n, u64(0));
    bit.build(n);
  }
  void build(vt<int> dat) {
    build(si(dat), [&](int i) -> int { return dat[i]; });
  }

  template <typename F>
  void build(int m, F f) {
    N = m;
    n = ceil<int>(N + 1, 64);
    dat.assign(n, u64(0));
    FOR(i, N) {
      u64 x = f(i);
      assert(x == 0 || x == 1);
      dat[i / 64] |= x << (i % 64);
    }
    bit.build(n, [&](int i) -> int { return popcnt(dat[i]); });
  }

  int sum_all() { return bit.sum_all(); }
  int sum(int k) { return prefix_sum(k); }
  int prefix_sum(int k) {
    assert(0 <= k && k <= N);
    int ans = bit.sum(k / 64);
    ans += popcnt(dat[k / 64] & ((u64(1) << (k % 64)) - 1));
    return ans;
  }
  int sum(int L, int R) {
    assert(0 <= L && L <= R && R <= N);
    if (L == 0) return prefix_sum(R);
    int ans = 0;
    ans -= popcnt(dat[L / 64] & ((u64(1) << (L % 64)) - 1));
    ans += popcnt(dat[R / 64] & ((u64(1) << (R % 64)) - 1));
    ans += bit.sum(L / 64, R / 64);
    return ans;
  }
  int get(int i) {
    assert(0 <= i && i < N);
    return sum(i, i + 1);
  }
  int prod(int L, int R) { return sum(L, R); }

  void add(int k, int x) {
    assert(0 <= k && k < N);
    if (x == 1) {
      assert(sum(k, k + 1) == 0);
      dat[k / 64] |= u64(1) << (k % 64);
      bit.add(k / 64, 1);
    } else if (x == -1) {
      assert(sum(k, k + 1) == 1);
      dat[k / 64] &= ~(u64(1) << (k % 64));
      bit.add(k / 64, -1);

    } else
      assert(false);
  }
  void multiply(int k, int x) { add(k, x); }

  void add(int k) { add(k, 1); }
  void remove(int k) { add(k, -1); }
};