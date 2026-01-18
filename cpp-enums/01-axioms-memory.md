# Part 01: Memory and Type Sizes - Verified Measurements

## Integer Storage: 4 Bytes

Source code enum_storage.cpp:
```cpp
#include <iostream>
enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

int main() {
  OrderType type1 = OrderType::SELL;
  OrderType2 type2 = OrderType2::SELL;
  std::cout << "sizeof(OrderType): " << sizeof(type1) << "\n";
  std::cout << "sizeof(OrderType2): " << sizeof(type2) << "\n";
  return 0;
}
```

Compilation: `clang++ -std=c++23 enum_storage.cpp -o enum_storage`

Execution output:
```
sizeof(OrderType): 4
sizeof(OrderType2): 1
```

Verification: int enum uses 4 bytes, char enum uses 1 byte. Measured directly via sizeof operator.

## ASCII Character Values

Test program:
```cpp
#include <iostream>
int main() {
  std::cout << static_cast<int>('B') << "\n";
  std::cout << static_cast<int>('S') << "\n";
  return 0;
}
```

Output:
```
66
83
```

Verification: 'B' = 66 decimal = 0x42 hex. 'S' = 83 decimal = 0x53 hex. ASCII encoding confirmed.

## Memory Efficiency Calculation

Array test:
```cpp
OrderType arr1[1000];     // sizeof = 4000 bytes
OrderType2 arr2[1000];    // sizeof = 1000 bytes
```

Savings: 4000 - 1000 = 3000 bytes. Percentage: 3000/4000 = 75% reduction. Verified by sizeof measurements.
