#include "book/template.hpp"
#include "book/io.hpp"

#include "book/graph/shortest_path/dijkstra.hpp"

void solve() {
  INT(N, Q);

  vi A(N), B(N);
  Graph<ll> G(N + 1);
  vv(pi, E, N + 1);
  FOR(i, 1, N + 1) {
    read(A[i - 1]);
    G.add(i - 1, i % N, A[i - 1]);
  }
  FOR(i, 1, N + 1) {
    read(B[i - 1]);
    G.add(i - 1, N, B[i - 1]);
  }

  auto dist = dijkstra<ll>(G, N);

  vi pref = cumsum<ll>(vector<ll>(A.begin(), A.begin() + N - 1));

  FOR(Q) {
    INT(s, t);
    s--;
    t--;

    ll ANS = dist[s] + dist[t];
    if (t != N) {
      ckmin(ANS, pref[t] - pref[s]);
      ckmin(ANS, pref[s] + A.back() + pref.back() - pref[t]);
    }
    print(ANS);
  }
}

int main() {
  solve();
  return 0;
}