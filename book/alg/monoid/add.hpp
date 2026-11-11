template <typename T>
struct Monoid_Add {
  using value_type = T;
  static constexpr T op(const T& a, const T& b) noexcept { return a + b; }
  static constexpr T inverse(const T& a) noexcept { return -a; }
  static constexpr T power(const T& a, ll n) noexcept { return T(n) * a; }
  static constexpr T e() { return T(0); }
  static constexpr bool commute = true;
};
