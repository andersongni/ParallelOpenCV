#include <iostream>
#include <vector>
#include <thread>

namespace {
  std::vector<std::thread> workers;

  int total = 4;
  int arr[4] = {0};

  void each_thread_does(int i) {
    arr[i] += 2;
  }
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 8; ++i) { // for 8 iterations,
    for (int j = 0; j < 4; ++j) {
      workers.push_back(std::thread(each_thread_does, j));
    }
    for (std::thread &t: workers) {
      if (t.joinable()) {
        t.join();
      }
    }
    arr[4] = std::min_element(arr, arr+4);
  }
  return 0;
}
