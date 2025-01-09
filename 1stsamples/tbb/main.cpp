#include <tbb/tbb.h>

#include <iostream>

namespace {
int FibFunc(int n) {
  if (n < 2) {
    return n;
  }
  int x;
  int y;
  oneapi::tbb::task_group g;
  g.run([&] { x = FibFunc(n - 1); });
  g.run([&] { y = FibFunc(n - 2); });
  g.wait();
  return x + y;
}
}  // namespace

int main() { return FibFunc(10) - 55; }
