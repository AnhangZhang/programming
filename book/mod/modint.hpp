#include "book/mod/modint_common.hpp"

template <int mod>
struct modint {
  static constexpr u32 umod = u32(mod);
  static_assert(0 < umod && umod < u32(1) << 31);
  u32 val;

  static modint raw(u32 v) {
    modint x;
    x.val = v;
    return x;
  }
  constexpr modint() : val(0) {}
  constexpr modint(u32 x) : val(x % umod) {}
  constexpr modint(u64 x) : val(x % umod) {}
  constexpr modint(u128 x) : val(x % umod) {}
  constexpr modint(int x) : val((x %= mod) < 0 ? x + mod : x) {}
  constexpr modint(ll x) : val((x %= mod) < 0 ? x + mod : x) {}
  constexpr modint(i128 x) : val((x %= mod) < 0 ? x + mod : x) {}
  bool operator<(const modint& other) const { return val < other.val; }
  modint& operator+=(const modint& p) {
    if ((val += p.val) >= umod) val -= umod;
    return *this;
  }
  modint& operator-=(const modint& p) {
    if ((val += umod - p.val) >= umod) val -= umod;
    return *this;
  }
  modint& operator*=(const modint& p) {
    val = u64(val) * p.val % umod;
    return *this;
  }
  modint& operator/=(const modint& p) {
    *this *= p.inverse();
    return *this;
  }
  modint operator-() const { return modint::raw(val ? mod - val : u32(0)); }
  modint operator+(const modint& p) const { return modint(*this) += p; }
  modint operator-(const modint& p) const { return modint(*this) -= p; }
  modint operator*(const modint& p) const { return modint(*this) *= p; }
  modint operator/(const modint& p) const { return modint(*this) /= p; }
  bool operator==(const modint& p) const { return val == p.val; }
  bool operator!=(const modint& p) const { return val != p.val; }
  modint inverse() const {
    int a = val, b = mod, u = 1, v = 0, t;
    while (b > 0) {
      t = a / b;
      swap(a -= t * b, b), swap(u -= t * v, v);
    }
    return modint(u);
  }
  modint pow(ll n) const {
    if (n < 0) return inverse().pow(-n);
    assert(n >= 0);
    modint ret(1), mul(val);
    while (n > 0) {
      if (n & 1) ret *= mul;
      mul *= mul;
      n >>= 1;
    }
    return ret;
  }
  static constexpr int get_mod() { return mod; }
};

#ifdef FASTIO
template <int mod>
void rd(modint<mod>& x) {
  fastio::rd(x.val);
  x.val %= mod;
}
template <int mod>
void wt(modint<mod> x) {
  fastio::wt(x.val);
}
#endif
using modint107 = modint<int(1e9) + 7>;
using modint998 = modint<998244353>;