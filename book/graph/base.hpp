// Adapted from https://github.com/maspypy/library/blob/main/graph/base.hpp
template <typename T>
struct Edge {
  int from, to;
  T cost;
  int id;
};

// 顶点从 0 开始编号；add 后即可遍历，无需额外 build()。
template <typename T = int, bool directed = false>
struct Graph {
  static constexpr bool is_directed = directed;
  using cost_type = T;
  using edge_type = Edge<T>;

  int N, M;
  vector<edge_type> edges;        // 每条原始边只保存一次
  vector<vector<edge_type>> adj;  // 无向边保存两个方向，共用 id

  Graph(int n = 0) { build(n); }

  // 清空图并重新设置顶点数。
  void build(int n) {
    assert(n >= 0);
    N = n, M = 0;
    edges.clear();
    adj.assign(N, {});
    indeg.assign(directed ? N : 0, 0);
  }

  void add(int from, int to, T cost = 1, int id = -1) {
    assert(0 <= from && from < N && 0 <= to && to < N);
    if (id == -1) id = M;
    edge_type e{from, to, cost, id};
    edges.push_back(e);
    adj[from].push_back(e);
    if constexpr (directed) {
      ++indeg[to];
    } else {
      adj[to].push_back({to, from, cost, id});
    }
    ++M;
  }

  const vector<edge_type>& operator[](int v) const {
    assert(0 <= v && v < N);
    return adj[v];
  }

  int out_deg(int v) const { return int((*this)[v].size()); }

  int in_deg(int v) const {
    assert(0 <= v && v < N);
    if constexpr (directed) return indeg[v];
    return out_deg(v);
  }

  // 有向图返回入度 + 出度；无向图的自环贡献 2。
  int deg(int v) const {
    if constexpr (directed) return in_deg(v) + out_deg(v);
    return out_deg(v);
  }

  vector<int> deg_array() const {
    vector<int> res(N);
    for (int v = 0; v < N; ++v) res[v] = deg(v);
    return res;
  }

  pair<vector<int>, vector<int>> deg_array_inout() const {
    vector<int> in(N), out(N);
    for (int v = 0; v < N; ++v) {
      in[v] = in_deg(v), out[v] = out_deg(v);
    }
    return {in, out};
  }

#ifdef FASTIO
  // wt 表示输入是否带边权；off=1 将输入的 1-based 顶点转为 0-based。
  void read_graph(int m, bool wt = false, int off = 1) {
    assert(m >= 0);
    for (int i = 0; i < m; ++i) {
      int a, b;
      read(a, b);
      T cost = 1;
      if (wt) read(cost);
      add(a - off, b - off, cost);
    }
  }

  void read_tree(bool wt = false, int off = 1) {
    assert(N > 0);
    read_graph(N - 1, wt, off);
  }
#endif

 private:
  vector<int> indeg;
};
