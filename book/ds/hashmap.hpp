// u64 -> T
template <typename T>
struct HashMap {
  // n想要插入的元素的数量
  HashMap(u32 n = 0) { build(n); }
  void build(u32 n) {
    u32 k = 8;
    while (k < n * 2) k *= 2;
    cap = k / 2, mask = k - 1;  // cap=k/2 最大装载率控制在 50%
    key.resize(k), val.resize(k), used.assign(k, 0);
  }

  void clear() {
    used.assign(si(used), 0);
    cap = (mask + 1) / 2;
  }
  int size() { return si(used) / 2 - cap; }

  int index(const u64& k) {
    int i = 0;
    for (i = hash(k); used[i] && key[i] != k; i = (i + 1) & mask);
    return i;
  }
  T& operator[](const u64& k) {
    int i = index(k);
    if (used[i]) return val[i];
    if (cap == 0) extend(), i = index(k);
    used[i] = true;
    key[i] = k, val[i] = T{}, --cap;
    return val[i];
  }
  T get(const u64& k, T default_value) {
    int i = index(k);
    if (used[i])
      return val[i];
    else
      return default_value;
  }
  bool count(const u64& k) {
    int i = index(k);
    return used[i] && key[i] == k;
  }

 private:
  u32 cap, mask;
  vt<u64> key;
  vt<T> val;
  vt<bool> used;

  u64 hash(u64 x) {
    static const u64 FIXED_RANDOM =
        std::chrono::steady_clock::now().time_since_epoch().count();
    x += FIXED_RANDOM;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return (x ^ (x >> 31)) & mask;
  }
  void extend() {
    vt<pair<u64, T>> dat;
    dat.reserve(si(used) / 2 - cap);
    FOR(i, si(used)) if (used[i]) dat.eb(key[i], val[i]);

    build(si(dat) * 2);
    for (auto& [a, b] : dat) (*this)[a] = b;
  };
};