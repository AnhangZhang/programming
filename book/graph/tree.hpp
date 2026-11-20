#include "graph/base.hpp"

template <typename GT, bool HLD = true>
struct Tree {
  using Graph_type = GT;
  using WT = typename GT::cost_type;
  int N = 0;
  vt<int> LID, RID, sz, hson, head, V, parent, VtoE, EtoV;
  vt<int> depth;
  vt<WT> depth_weighted;
  vt<int> memo_tail;

  Tree() = default;
  Tree(const GT& G, int r = 0) { build(G, r); }

  void build(const GT& G, int r = 0) {
    N = G.N;
    assert(G.M == N - 1);
    FOR(i, N - 1) assert(G.edges[i].id == i);
    assert(0 <= r && r < N);
    if constexpr (!HLD)
      build_simple(G, r);
    else
      build_HLD(G, r);
  }

  // a is in b
  bool in_subtree(int a, int b) const {
    return LID[b] <= LID[a] && LID[a] < RID[b];
  }

  int LCA(int u, int v) const {
    static_assert(HLD);

    while (head[u] != head[v]) {
      if (depth[head[u]] < depth[head[v]]) swap(u, v);
      u = parent[head[u]];
    }
    return depth[u] < depth[v] ? u : v;
  }

 private:
  void build_simple(const GT& G, int r = 0) {
    N = G.N;
    LID.assign(N, 0), RID.assign(N, 0);
    V.assign(N, -1), parent.assign(N, -1), VtoE.assign(N, -1),
        EtoV.assign(N - 1, -1);
    sz.assign(N, 0), depth.assign(N, 0), depth_weighted.assign(N, 0);

    int k = 0;
    auto dfs = [&](auto& self, int u, int p) -> void {
      LID[u] = k;
      V[k] = u;
      ++k;
      sz[u] = 1;
      for (auto e : G[u]) {
        int v = e.to;
        if (v == p) continue;
        parent[v] = u;
        depth[v] = depth[u] + 1;
        depth_weighted[v] = depth_weighted[u] + e.cost;
        VtoE[v] = e.id, EtoV[e.id] = v;
        self(self, v, u);
        sz[u] += sz[v];
      }
      RID[u] = k;
    };
    dfs(dfs, r, -1);
  }
  void build_HLD(const GT& G, int r = 0) {
    N = G.N;
    LID.assign(N, 0), RID.assign(N, 0), head.assign(N, r);
    V.assign(N, -1), parent.assign(N, -1), VtoE.assign(N, -1),
        EtoV.assign(N - 1, -1);
    sz.assign(N, 0), hson.assign(N, -1), depth.assign(N, 0),
        depth_weighted.assign(N, 0);

    memo_tail.clear();
    auto dfs1 = [&](auto& self, int u, int p) -> void {
      sz[u] = 1;
      int max_sz = 0;
      for (const auto& e : G[u]) {
        int v = e.to;
        if (v == p) continue;
        parent[v] = u;
        depth[v] = depth[u] + 1;
        depth_weighted[v] = depth_weighted[u] + e.cost;
        VtoE[v] = e.id, EtoV[e.id] = v;
        self(self, v, u);
        sz[u] += sz[v];
        if (ckmax(max_sz, sz[v])) hson[u] = v;
      }
    };
    dfs1(dfs1, r, -1);

    int k = 0;
    auto dfs2 = [&](auto& self, int u) -> void {
      LID[u] = k;
      V[k++] = u;
      if (hson[u] != -1) {
        head[hson[u]] = head[u];
        self(self, hson[u]);
      }
      for (const auto& e : G[u]) {
        int v = e.to;
        if (v == parent[u] || v == hson[u]) continue;
        head[v] = v;
        self(self, v);
      }
      RID[u] = k;
    };
    dfs2(dfs2, r);
  }
};
