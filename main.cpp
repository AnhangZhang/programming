#include "template.hpp"
#include "io.hpp"

void solve() {
  LL(N, S, T);
  using TL = tuple<ll, ll, ll, ll>;
  VEC(TL, L, N);

  db ANS = infty<db>;
  vi ord(N);
  iota(all(ord), 0);

  auto dis = [&](ll x, ll y, ll a, ll b) -> db {
    return sqrt((x - a) * (x - a) + (y - b) * (y - b));
  };
  do {
    ll x = 0, y = 0;
    db cur = 0;
    FOR(i, N) {
      auto [a, b, c, d] = L[ord[i]];
      if (dis(x, y, a, b) < dis(x, y, c, d)) {
        cur += dis(x, y, a, b);
        x = a, y = b;
      } else {
        cur += dis(x, y, c, d);
        x = c, y = d;
      }
      cur += dis(a, b, c, d);
    }
    ckmin(ANS, cur);
  } while (next_permutation(all(ord)));

  print(ANS);
}

int main() {
  solve();
  return 0;
}
