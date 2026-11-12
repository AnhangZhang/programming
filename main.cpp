#include "book/template.hpp"
#include "book/io.hpp"

void solve() {
  LL(N, Q);
  VEC(ll, P, N);
  FOR(Q) {
    LL(A);
    P.eb(A);
  }

  vi ANS;
  vt<bool> used(N + 1);
  FOR_R(i, si(P)) {
    if (used[P[i]]) continue;
    used[P[i]] = true;
    ANS.eb(P[i]);
  }
  reverse(all(ANS));
  print(ANS);
}

int main() {
  solve();
  return 0;
}