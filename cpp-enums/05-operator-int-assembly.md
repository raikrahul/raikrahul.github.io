# Part 05: operator<<(int) Assembly Analysis

## Source Code

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

## Assembly Generation

Command: `clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s`

Generated assembly:
```asm
movl    $0, -4(%rbp)              # return value = 0
movl    $0, -8(%rbp)              # type = OrderType::BUY (0)
movl    -8(%rbp), %esi            # load type into %esi (parameter)
movq    _ZSt4cout@GOTPCREL(%rip), %rdi  # load cout address (this pointer)
callq   _ZNSolsEi@PLT             # call cout.operator<<(int)
xorl    %eax, %eax                # return 0
addq    $16, %rsp
popq    %rbp
retq
```

## Instruction Breakdown

1. `movl $0, -8(%rbp)` - Store enum value 0 on stack (4 bytes)
2. `movl -8(%rbp), %esi` - Load value into register %esi (2nd parameter)
3. `movq _ZSt4cout@GOTPCREL(%rip), %rdi` - Load cout object address into %rdi (1st parameter)
4. `callq _ZNSolsEi@PLT` - Call operator<<(int) function

## Function Name Demangling

Command: `c++filt _ZNSolsEi`
Output: `std::ostream::operator<<(int)`

Verification: Mangled name _ZNSolsEi = operator<<(int) member function.

## Parameter Passing (x86-64 Calling Convention)

Register %rdi = 1st parameter = this pointer (cout object address)
Register %rsi = 2nd parameter = int value to print

Function signature: `ostream& operator<<(ostream* this, int value)`

## Machine Code

Command: `objdump -d test_enum | grep -A10 "main>:"`

Output:
```
1178:   48 8b 3d 49 2e 00 00    mov    0x2e49(%rip),%rdi
117f:   e8 bc fe ff ff          call   1040 <_ZNSolsEi@plt>
```

Bytes for call instruction: `e8 bc fe ff ff` (5 bytes)

## libstdc++ Source Reference

File: /usr/include/c++/13/ostream
Line 191: `operator<<(int __n);`

Implementation in libstdc++.so performs:
1. Sign checking
2. Digit extraction via division
3. ASCII conversion (digit + '0')
4. Buffer writes

Complexity: O(log₁₀ n) where n is the integer value.

## Instruction Count

Main function operations:
- Store enum: 1 instruction
- Load value: 1 instruction  
- Load cout: 1 instruction
- Call operator<<: 1 instruction
- Inside operator<<: ~50+ instructions for conversion

Total: ~54 instructions per print operation.
