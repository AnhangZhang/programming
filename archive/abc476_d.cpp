#include "book/template.hpp"
#include "book/io.hpp"

void solve() {
  LL(N, M, K);
  LL(X, Y);
  VEC(ll, A, N);
  A.insert(A.begin(), 0);
  N = si(A);
  VEC(ll, B, M);
  vi pref(M + 1);
  FOR(i, M) pref[i + 1] = pref[i] + ceil(B[i], K);

  sort(all(A));
  sort(all(B));

  ll curX = X, curY = Y;
  ll ANS = 0;
  FOR(i, N) {
    if (curX >= A[i]) {
      curX -= A[i];
    } else if (curY * K + curX >= A[i]) {
      ll rem = A[i] - curX;
      curX = (K - (rem % K)) % K;
      curY -= rem / K + 1;
    } else {
      break;
    }

    int low = 0, high = M;
    while (low < high) {
      int mid = (low + high + 1) / 2;
      if (curY >= pref[mid])
        low = mid;
      else
        high = mid - 1;
    }
    ckmax(ANS, low + i);
  }

  print(ANS);
}

int main() {
  solve();
  return 0;
}