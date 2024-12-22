#include <atomic>
#include <iostream>

int main() {
  std::atomic<int> i;

  i.store(1, std::memory_order_release);
  i = 3;
  std::cout << i << std::endl;  // 3
  return 0;
}