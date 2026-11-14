template <typename mint>
mint fact(int n) {
  static vector<mint> dat = {1, 1};
  static int mod = 0;
  if (mod != mint::get_mod()) {
    mod = mint::get_mod();
    dat = {1, 1};
  }
  assert(0 <= n && n < mod);
  if (si(dat) <= n) {
    int now = si(dat);
    int m = min(mod, 1 << (topbit(n) + 1));
    dat.resize(m);
    FOR(i, now, m) dat[i] = dat[i - 1] * mint::raw(i);
  }
  return dat[n];
}

template <typename mint>
mint fact_inv(int n) {
  if (n < 0) return mint(0);
  static vector<mint> dat = {1, 1};
  static int mod = 0;
  if (mod != mint::get_mod()) {
    mod = mint::get_mod();
    dat = {1, 1};
  }
  assert(0 <= n && n < mod);
  if (si(dat) <= n) {
    int now = si(dat);
    int m = min(mod, 1 << (topbit(n) + 1));
    dat.resize(m);
    dat[m - 1] = fact<mint>(m - 1).inverse();
    FOR_R(i, now, m - 1) dat[i] = dat[i + 1] * mint::raw(i + 1);
  }
  return dat[n];
}

template <typename mint>
mint C(int n, int k) {
  if (k < 0 || n < k) return 0;
  return fact<mint>(n) * fact_inv<mint>(k) * fact_inv<mint>(n - k);
}
