#include "book/template.hpp"
#include "book/io.hpp"

void solve() {
  LL(N);
  vi A(N), B(N);
  FOR(i, N) read(A[i], B[i]);

  vi ord(N);
  iota(all(ord), 0);
  sort(all(ord), [&](int i, int j) { return A[i] - B[i] > A[j] - B[j]; });

  ll mi = MIN(A);
  ll base = SUM<ll>(A);
  ll ANS = base;

  ll rem = 0;
  FOR(i, N - 1) {
    // need i+1 coupons
    rem += A[ord[i]] - B[ord[i]];
    ll extra = max(0ll, (2 * (i + 1) - N)) * mi;
    ckmin(ANS, base + extra - rem);
  }
  print(ANS);
}

int main() {
  INT(T);
  FOR(T)
  solve();
  return 0;
}