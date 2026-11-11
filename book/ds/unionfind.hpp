struct UnionFind {
  int n, cnt;
  vt<int> dat;
  UnionFind(int n = 0) { build(n); }

  void build(int m) {
    n = m, cnt = m;
    dat.assign(n, -1);
  }
  int operator[](int x) {
    while (dat[x] >= 0) {
      int x2 = dat[dat[x]];
      if (x2 < 0) {
        return dat[x];
      }
      x = dat[x] = x2;
    }
    return x;
  }
  bool merge(int x, int y) {
    x = (*this)[x], y = (*this)[y];
    if (x == y) return false;
    if (-dat[x] < -dat[y]) swap(x, y);
    dat[x] += dat[y];
    dat[y] = x;
    cnt--;
    return true;
  }
  ll size(int x) {
    x = (*this)[x];
    return -dat[x];
  }
  vt<int> get_all() {
    vt<int> A(n);
    FOR(i, n) A[i] = (*this)[i];
    return A;
  }
};