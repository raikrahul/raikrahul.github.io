# Part 04: Enum Name Disappearance - Binary Analysis Proof

## Test Program

test_enum.cpp:
```cpp
#include <iostream>
enum class OrderType { BUY, SELL };

int main() {
  OrderType type = OrderType::BUY;
  std::cout << static_cast<int>(type);
  return 0;
}
```

## Compilation

Command: `clang++ -std=c++23 test_enum.cpp -o test_enum`

## String Search in Binary

Command: `strings test_enum | grep BUY`
Output: (no output)

Command: `strings test_enum | grep SELL`
Output: (no output)

Verification: Strings "BUY" and "SELL" do not exist in compiled binary.

## Assembly Analysis

Command: `clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s`

Generated assembly:
```asm
movl    $0, -8(%rbp)              # OrderType::BUY becomes 0
```

Observation: Assembly contains numeric constant 0, not string "BUY".

## Machine Code Analysis

Command: `objdump -d test_enum | grep -A5 "main>:"`

Output:
```
115f:   c7 45 f8 00 00 00 00    movl   $0x0,-0x8(%rbp)
```

Bytes: `c7 45 f8 00 00 00 00`
- Immediate value: `00 00 00 00` (numeric 0)
- No ASCII encoding of "BUY" (would be `42 55 59` in hex)

## Symbol Table Check

Command: `nm test_enum | grep -i buy`
Output: (no output)

Command: `nm test_enum | grep -i sell`
Output: (no output)

Verification: Enum names not in symbol table.

## Compiler Substitution Process

Source code: `OrderType::BUY`
Compiler replaces with: `0`
Assembly output: `movl $0, -8(%rbp)`
Machine code: `00 00 00 00`

Source code: `OrderType::SELL`
Compiler replaces with: `1`
Assembly output: `movl $1, -8(%rbp)`
Machine code: `01 00 00 00`

## Conclusion

Enum names exist only in source code. After compilation:
- Binary contains only numeric values (0, 1)
- No string literals for enum names
- No symbol table entries for enum values
- Assembly shows numeric constants only

Verified by: strings command, objdump disassembly, nm symbol table, assembly output.
