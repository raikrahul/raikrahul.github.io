# Part 02: Assembly Instructions - Actual Generated Code

## Source Code

test_enum.cpp:
```cpp
#include <iostream>
enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

int main() {
  OrderType type = OrderType::SELL;
  OrderType2 type2 = OrderType2::SELL;
  std::cout << static_cast<int>(type);
  std::cout << static_cast<char>(type2);
  return 0;
}
```

## Compilation and Assembly Generation

Command: `clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s`

## Generated Assembly for int enum

From test_enum.s:
```asm
movl    $1, -8(%rbp)              # type = OrderType::SELL (1)
```

Instruction breakdown:
- `movl` = move long (4 bytes)
- `$1` = immediate value 1
- `-8(%rbp)` = stack location 8 bytes before base pointer

Stores 4-byte integer value 1 at stack address.

## Generated Assembly for char enum

From test_enum.s:
```asm
movb    $83, -9(%rbp)             # type2 = OrderType2::SELL (83)
```

Instruction breakdown:
- `movb` = move byte (1 byte)
- `$83` = immediate value 83 (0x53 = 'S')
- `-9(%rbp)` = stack location 9 bytes before base pointer

Stores 1-byte character value 83 at stack address.

## Machine Code Verification

Command: `clang++ -std=c++23 -O0 test_enum.cpp -o test_enum`
Command: `objdump -d test_enum | grep -A10 "main>:"`

Output showing int enum storage:
```
115f:   c7 45 f8 01 00 00 00    movl   $0x1,-0x8(%rbp)
```

Bytes: `c7 45 f8 01 00 00 00`
- `c7` = movl opcode
- `45 f8` = addressing mode and displacement
- `01 00 00 00` = 4-byte immediate value (1 in little-endian)

Output showing char enum storage:
```
1166:   c6 45 f7 53             movb   $0x53,-0x9(%rbp)
```

Bytes: `c6 45 f7 53`
- `c6` = movb opcode
- `45 f7` = addressing mode and displacement
- `53` = 1-byte immediate value (83)

## Instruction Size Comparison

movl instruction: 7 bytes total (opcode + addressing + 4-byte immediate)
movb instruction: 4 bytes total (opcode + addressing + 1-byte immediate)

Verification: movl operates on 4 bytes, movb operates on 1 byte.
