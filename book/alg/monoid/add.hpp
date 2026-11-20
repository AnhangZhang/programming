template <typename E>
struct Monoid_Add {
  using X = E;
  using value_type = X;
  static constexpr X op(const X& a, const X& b) noexcept { return a + b; }
  static constexpr X inverse(const X& a) noexcept { return -a; }
  static constexpr X power(const X& a, ll n) noexcept { return X(n) * a; }
  static constexpr X e() { return X(0); }
  static constexpr bool commute = true;
};
