#include <chrono>
#include <fstream>
#include <iostream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY = 'B', SELL = 'S' };

constexpr char to_char(OrderType o) noexcept {
  return o == OrderType::BUY ? 'B' : 'S';
}

void benchmark_int_enum() {
  auto start = std::chrono::high_resolution_clock::now();

  std::ofstream null_stream("/dev/null");
  for (int i = 0; i < 10000000; ++i) {
    OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
    null_stream << static_cast<int>(type);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "int enum (cast to int): " << duration.count() << " ms\n";
}

void benchmark_int_enum_with_function() {
  auto start = std::chrono::high_resolution_clock::now();

  std::ofstream null_stream("/dev/null");
  for (int i = 0; i < 10000000; ++i) {
    OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
    null_stream << to_char(type);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "int enum (to_char function): " << duration.count() << " ms\n";
}

void benchmark_char_enum() {
  auto start = std::chrono::high_resolution_clock::now();

  std::ofstream null_stream("/dev/null");
  for (int i = 0; i < 10000000; ++i) {
    OrderType2 type = (i & 1) ? OrderType2::SELL : OrderType2::BUY;
    null_stream << static_cast<char>(type);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "char enum (direct cast): " << duration.count() << " ms\n";
}

int main() {
  std::cout << "Running benchmarks (no optimization)...\n\n";

  benchmark_int_enum();
  benchmark_int_enum_with_function();
  benchmark_char_enum();

  return 0;
}
