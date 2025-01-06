#include <iostream>
#include <string>
#include <thread>
#include <vector>

static void Task(const std::string& msg) { std::cout << "thread number: " + msg << '\n'; }

int main() {
  const auto num_max_threads = std::thread::hardware_concurrency();
  std::cout << "Number of threads = " << num_max_threads << '\n';
  std::vector<std::thread> thr(num_max_threads);

  for (unsigned int i = 0; i < num_max_threads; ++i) {
    thr[i] = std::thread(Task, std::to_string(i));
  }

  for (unsigned int i = 0; i < num_max_threads; ++i) {
    thr[i].join();
  }
  return 0;
}