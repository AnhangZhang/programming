#include "book/template.hpp"
#include "book/io.hpp"

#include "book/ds/fenwicktree.hpp"

const int MAX = 500010;
void solve() {
  LL(N);
  FenwickTree<Monoid_Add<ll>> FT(MAX);
  ll sum = 0;
  FOR(i, N) {
    INT(a, b);
    sum += a;
    int gap = b - a;
    assert(0 <= gap && gap < MAX);
    FT.add(gap, b);
  }
  LL(Q);
  FOR(Q) {
    INT(x);
    ll ans = sum + (ll)x * N;
    ll rem = FT.prod(x + 1);
    // 当需要 mod 时，有种情况只 -b 不够。
    SHOW(rem);
    print(ans - rem);
  }
}

int main() {
  solve();
  return 0;
}