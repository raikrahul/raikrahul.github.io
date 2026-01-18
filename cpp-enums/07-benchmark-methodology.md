# Part 07: Benchmark Methodology

## Benchmark Source Code

enum_benchmark.cpp:
```cpp
#include <chrono>
#include <iostream>
#include <fstream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "char enum (direct cast): " << duration.count() << " ms\n";
}

int main() {
    std::cout << "Running benchmarks (no optimization)...\n\n";
    
    benchmark_int_enum();
    benchmark_int_enum_with_function();
    benchmark_char_enum();
    
    return 0;
}
```

## Compilation

Command: `clang++ -std=c++23 -O0 enum_benchmark.cpp -o enum_benchmark`

Flags:
- `-std=c++23` - C++23 standard
- `-O0` - No optimization (fair comparison)

## Test Configuration

Iterations: 10,000,000 per test
Output: /dev/null (eliminates I/O bottleneck)
Timing: std::chrono::high_resolution_clock
Resolution: milliseconds

## Execution

Command: `./enum_benchmark`

## Measurement Methodology

1. Start timer before loop
2. Execute 10M iterations
3. Stop timer after loop
4. Calculate duration in milliseconds
5. Output result

Per-operation cost calculation:
```
time_per_operation = total_ms / 10,000,000
```

Example: 335ms / 10,000,000 = 0.0000335ms = 33.5 nanoseconds

## Test Variations

Test 1: int enum with static_cast<int>
- Stores 4-byte int
- Calls operator<<(int)
- Performs int-to-string conversion

Test 2: int enum with to_char() function
- Stores 4-byte int
- Calls to_char() function (adds overhead)
- Returns char
- Calls operator<<(char)

Test 3: char enum with static_cast<char>
- Stores 1-byte char
- Calls operator<<(char) directly
- No conversion needed
