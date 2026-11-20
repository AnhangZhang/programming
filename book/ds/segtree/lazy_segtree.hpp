template <typename ActedMonoid>
struct Lazy_SegTree {
  using AM = ActedMonoid;
  using MX = typename AM::Monoid_X;
  using MA = typename AM::Monoid_A;
  using X = typename MX::value_type;
  using A = typename MA::value_type;
  int n, log, size;
  vt<X> dat;
  vt<A> laz;
  vt<bool> has_laz;

  Lazy_SegTree() {}
  Lazy_SegTree(int n) { build(n); }
  template <typename F>
  Lazy_SegTree(int n, F f) {
    build(n, f);
  }
  Lazy_SegTree(const vt<X>& v) { build(v); }

  void build(int m) {
    build(m, [](int i) -> X { return MX::e(); });
  }
  void build(const vt<X>& v) {
    build(si(v), [&](int i) -> X { return v[i]; });
  }
  template <typename F>
  void build(int m, F f) {
    n = m, log = 0;
    while ((1 << log) < n) ++log;
    size = 1 << log;
    dat.assign(size << 1, MX::e());
    laz.assign(size, MA::e());
    has_laz.assign(size, false);
    FOR(i, n) dat[size + i] = f(i);
    FOR_R(i, 1, size) update(i);
  }

  void update(int k) { dat[k] = MX::op(dat[2 * k], dat[2 * k + 1]); }
  void set(int p, X x) {
    assert(0 <= p && p < n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    dat[p] = x;
    for (int i = 1; i <= log; i++) update(p >> i);
  }
  void multiply(int p, const X& x) {
    assert(0 <= p && p < n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    dat[p] = MX::op(dat[p], x);
    for (int i = 1; i <= log; i++) update(p >> i);
  }

  X get(int p) {
    assert(0 <= p && p < n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    return dat[p];
  }
  vt<X> get_all() {
    FOR(k, 1, size) { push(k); }
    return {dat.begin() + size, dat.begin() + size + n};
  }
  X prod(int l, int r) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r) return MX::e();
    l += size, r += size;
    for (int i = log; i >= 1; i--) {
      if (((l >> i) << i) != l) push(l >> i);
      if (((r >> i) << i) != r) push((r - 1) >> i);
    }
    X xl = MX::e(), xr = MX::e();
    while (l < r) {
      if (l & 1) xl = MX::op(xl, dat[l++]);
      if (r & 1) xr = MX::op(dat[--r], xr);
      l >>= 1, r >>= 1;
    }
    return MX::op(xl, xr);
  }
  X prod_all() { return dat[1]; }
  void apply(int l, int r, A a) {
    assert(0 <= l && l <= r && r <= n);
    if (l == r) return;
    l += size, r += size;
    for (int i = log; i >= 1; i--) {
      if (((l >> i) << i) != l) push(l >> i);
      if (((r >> i) << i) != r) push((r - 1) >> i);
    }
    int l2 = l, r2 = r;
    while (l < r) {
      if (l & 1) apply_at(l++, a);
      if (r & 1) apply_at(--r, a);
      l >>= 1, r >>= 1;
    }
    l = l2, r = r2;
    for (int i = 1; i <= log; i++) {
      if (((l >> i) << i) != l) update(l >> i);
      if (((r >> i) << i) != r) update((r - 1) >> i);
    }
  }

 private:
  void apply_at(int k, A a) {
    ll sz = 1 << (log - topbit(k));
    dat[k] = AM::act(dat[k], a, sz);
    if (k < size) has_laz[k] = 1, laz[k] = MA::op(laz[k], a);
  }
  void push(int k) {
    if (!has_laz[k]) return;
    has_laz[k] = 0;
    apply_at(2 * k, laz[k]), apply_at(2 * k + 1, laz[k]);
    laz[k] = MA::e();
  }
};