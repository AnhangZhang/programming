#include "graph/base.hpp"

// 对跨分量的边 u -> v，有 comp[u] < comp[v]。
template <typename GT>
pair<int, vt<int>> strongly_connected_component(const GT& G) {
  static_assert(GT::is_directed);
  int N = G.N, nc = 0, now = 0;
  vt<int> comp(N), low(N), ord(N, -1), path;
  path.reserve(N);

  auto dfs = [&](auto& dfs, int v) -> void {
    low[v] = ord[v] = now++;
    path.eb(v);
    for (const auto& e : G[v]) {
      int to = e.to;
      if (ord[to] == -1) {
        dfs(dfs, to);
        ckmin(low[v], low[to]);
      } else {
        ckmin(low[v], ord[to]);
      }
    }
    if (low[v] == ord[v]) {
      while (true) {
        int u = POP(path);
        // 已出栈顶点不再影响后续 low 值。
        ord[u] = N;
        comp[u] = nc;
        if (u == v) break;
      }
      ++nc;
    }
  };

  FOR(s, N) {
    if (ord[s] == -1) dfs(dfs, s);
  }
  for (int& c : comp) c = nc - 1 - c;
  return {nc, comp};
}

// 缩点为无权有向图，去掉自环和重边；nc、comp 使用上面函数的返回值。
template <typename GT>
Graph<int, true> scc_dag(const GT& G, int nc, const vt<int>& comp) {
  static_assert(GT::is_directed);
  Graph<int, true> dag(nc);
  vvt<int> edges(nc);
  for (const auto& e : G.edges) {
    int a = comp[e.from], b = comp[e.to];
    if (a != b) edges[a].eb(b);
  }
  for (int c = 0; c < nc; ++c) {
    UNIQUE(edges[c]);
    for (int to : edges[c]) dag.add(c, to);
  }
  return dag;
}
