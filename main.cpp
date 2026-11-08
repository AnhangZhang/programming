#include "book/template.hpp"
#include "book/io.hpp"

void solve() {
  int n;
  cin >> n;
  if (n % 2 == 1)
    cout << n / 2 + 1 << endl;
  else
    cout << n / 2 << endl;
  return;
}

int main() {
  solve();
  return 0;
}