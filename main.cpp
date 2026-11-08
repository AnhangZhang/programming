#include "book/template.hpp"
#include "book/io.hpp"

void solve() {
  int N, Q;
  cin >> N >> Q;
  vector<int> A(N);
  iota(A.begin(), A.end(), 1);
  for (int i = 0; i < Q; i++) {
    int k;
    cin >> k;
    for (int j = 0; j < k; j++) {
      A[j] = -A[j];
    }
    reverse(A.begin(), A.begin() + k);
  }
  for (int i = 0; i < N; i++) {
    cout << A[i] << endl;
  }
}

int main() {
  solve();
  return 0;
}