#include "graph/base.hpp"

template <typename T, typename GT>
pair<vt<T>, vt<int>> dijkstra(GT& G, int u) {
  auto N = G.N;
  vt<T> dist(N, infty<T>);
  min_pq<pair<T, int>> que;
  vector<int> par(N, -1);

  dist[u] = 0;
  que.emplace(dist[u], u);
  while (!que.empty()) {
    auto [exp, u] = POP(que);
    if (exp != dist[u]) continue;
    for (auto&& e : G[u]) {
      if (ckmin(dist[e.to], dist[e.from] + e.cost)) {
        par[e.to] = e.from;
        que.emplace(dist[e.to], e.to);
      }
    }
  }
  return {dist, par};
}