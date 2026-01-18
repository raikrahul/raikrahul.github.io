# Part 03: Enum Storage Verification - sizeof and Assembly Proof

## Test Program

enum_storage.cpp:
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

## Compilation and Execution

Command: `clang++ -std=c++23 enum_storage.cpp -o enum_storage`
Command: `./enum_storage`

Output:
```
sizeof(OrderType): 4
sizeof(OrderType2): 1
```

Verification: int enum = 4 bytes, char enum = 1 byte.

## Assembly Proof

Command: `clang++ -std=c++23 -S -O0 enum_storage.cpp -o enum_storage.s`

For OrderType (int enum):
```asm
movl    $1, -8(%rbp)
```
Uses movl (4-byte instruction).

For OrderType2 (char enum):
```asm
movb    $83, -9(%rbp)
```
Uses movb (1-byte instruction).

## Machine Code Verification

Command: `objdump -d enum_storage | grep -A10 "main>:"`

Output excerpt:
```
115f:   c7 45 f8 01 00 00 00    movl   $0x1,-0x8(%rbp)
1166:   c6 45 f7 53             movb   $0x53,-0x9(%rbp)
```

Analysis:
- movl immediate: `01 00 00 00` (4 bytes)
- movb immediate: `53` (1 byte)

## Array Storage Test

Code:
```cpp
OrderType arr1[1000];
OrderType2 arr2[1000];
std::cout << sizeof(arr1) << "\n";  // 4000
std::cout << sizeof(arr2) << "\n";  // 1000
```

Result: 4000 bytes vs 1000 bytes = 75% memory reduction.

## Conclusion

int enum uses 4× memory of char enum. Verified by:
1. sizeof operator measurements
2. Assembly instruction types (movl vs movb)
3. Machine code immediate operand sizes
4. Array storage calculations
