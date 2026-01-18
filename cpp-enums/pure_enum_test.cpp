#include <chrono>
#include <iostream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

volatile int sink_int = 0;
volatile char sink_char = 0;

void test_int_enum() {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100000000; ++i) {
        OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
        sink_int = static_cast<int>(type);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "int enum: " << duration.count() << " ms\n";
}

void test_char_enum() {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100000000; ++i) {
        OrderType2 type = (i & 1) ? OrderType2::SELL : OrderType2::BUY;
        sink_char = static_cast<char>(type);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "char enum: " << duration.count() << " ms\n";
}

int main() {
    std::cout << "Pure enum test (no cout, no conversion)\n\n";
    
    test_int_enum();
    test_char_enum();
    
    return 0;
}
