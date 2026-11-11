#include "book/graph/base.hpp"

template <typename T, typename GT>
vt<T> dijkstra(GT& G, int u) {
  auto N = G.N;
  vt<T> dist(N, infty<T>);
  min_pq<pair<T, int>> que;

  dist[u] = 0;
  que.emplace(dist[u], u);
  while (!que.empty()) {
    auto [exp, u] = POP(que);
    if (exp != dist[u]) continue;
    for (auto&& e : G[u]) {
      if (ckmin(dist[e.to], dist[e.from] + e.cost)) {
        que.emplace(dist[e.to], e.to);
      }
    }
  }
  return dist;
}