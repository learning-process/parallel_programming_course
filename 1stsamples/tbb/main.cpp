// Copyright 2023 Nesterov Alexander
#include <tbb/tbb.h>

#include <iostream>

int fib_func(int n) {
  if (n < 2) {
    return n;
  }
  int x;
  int y;
  oneapi::tbb::task_group g;
  g.run([&] { x = fib_func(n - 1); });
  g.run([&] { y = fib_func(n - 2); });
  g.wait();
  return x + y;
}

int main() { return fib_func(10) - 55; }
