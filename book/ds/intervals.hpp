template <typename T, typename X = ll>
struct Intervals {
  static constexpr X LLIM = -infty<X>;
  static constexpr X RLIM = infty<X>;
  T none_val;
  int total_num;
  X total_len;
  map<X, T> dat;

  Intervals(T none_val = 0) : none_val(none_val), total_num(0), total_len(0) {
    dat[LLIM] = none_val;
    dat[RLIM] = none_val;
  }

  // 得到 x 所在区间的信息 [l, r), val
  tuple<X, X, T> get(X x, bool earse = false) {
    auto it2 = dat.upper_bound(x);
    auto it1 = prev(it2);
    auto [l, tl] = *it1;
    auto [r, tr] = *it2;
    if (tl != none_val && erase) {
      --total_num;
      total_len -= r - l;
      dat[l] = none_val;
      merge_at(l);
      merge_at(r);
    }
    return {l, r, tl};
  }

  // f -> [l, r), t
  template <typename F>
  void enumerate_range(X l, X r, F f, bool erase = false) {
    assert(LLIM <= l && l <= r && r <= RLIM);
    if (!erase) {
      auto it = prev(dat.upper_bound(l));
      while ((*it).f < r) {
        auto it2 = next(it);
        f(max((*it).f, l), min((*it2).f, r), (*it).s);
        it = it2;
      }
      return;
    }
    auto p = prev(dat.upper_bound(l));
    if ((*p).f < l) {
      dat[l] = (*p).s;
      if (dat[l] != none_val) ++total_num;
    }
    p = dat.lower_bound(r);
    if (r < (*p).f) {
      T t = (*prev(p)).s;
      dat[r] = t;
      if (t != none_val) ++total_num;
    }
    p = dat.lower_bound(l);
    while (true) {
      if ((*p).f >= r) break;
      auto q = next(p);
      T t = (*p).s;
      f((*p).f, (*q).f, t);
      if (t != none_val) --total_num, total_len -= (*q).f - (*p).f;
      p = dat.erase(p);
    }
    dat[l] = none_val;
  }

  void set(X l, X r, T t) {
    assert(l <= r);
    if (l == r) return;
    enumerate_range(
        l, r,
        []([[maybe_unused]] int nl, [[maybe_unused]] int nr,
            [[maybe_unused]] T x) -> void {},
        true);
    dat[l] = t;
    if (t != none_val) total_num++, total_len += r - l;
    merge_at(l);
    merge_at(r);
  }

  void merge_at(X p) {
    if (p == LLIM || RLIM == p) return;
    auto itp = dat.lower_bound(p);
    assert((*itp).f == p);
    auto itq = prev(itp);
    if ((*itp).s == (*itq).s) {
      if ((*itp).s != none_val) --total_num;
      dat.erase(itp);
    }
  }
};