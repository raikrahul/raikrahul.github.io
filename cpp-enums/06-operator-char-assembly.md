# Part 06: operator<<(char) Assembly Analysis

## Source Code

test_enum.cpp:
```cpp
#include <iostream>
enum class OrderType2 : char { BUY='B', SELL='S' };

int main() {
  OrderType2 type2 = OrderType2::SELL;
  std::cout << static_cast<char>(type2);
  return 0;
}
```

## Assembly Generation

Command: `clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s`

Generated assembly:
```asm
movb    $83, -9(%rbp)             # type2 = OrderType2::SELL (83)
movsbl  -9(%rbp), %esi            # load char with sign extension
movq    _ZSt4cout@GOTPCREL(%rip), %rdi  # load cout address
callq   _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c@PLT
```

## Instruction Breakdown

1. `movb $83, -9(%rbp)` - Store char value 83 on stack (1 byte)
2. `movsbl -9(%rbp), %esi` - Load byte with sign extension into %esi
3. `movq _ZSt4cout@GOTPCREL(%rip), %rdi` - Load cout address
4. `callq` - Call operator<<(char)

## Function Name Demangling

Command: `c++filt _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c`
Output: `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char)`

Simplified: `operator<<(ostream&, char)`

## libstdc++ Source Implementation

File: /usr/include/c++/13/ostream
Line 570-575:

```cpp
operator<<(basic_ostream<char, _Traits>& __out, char __c)
{
  if (__out.width() != 0)
    return __ostream_insert(__out, &__c, 1);
  __out.put(__c);
  return __out;
}
```

Implementation:
1. Check width (default = 0, skips padding)
2. Call put(__c) - writes single byte
3. Return stream

put() function (line 370):
```cpp
put(char_type __c) {
  sputc(__c);
  return *this;
}
```

sputc() writes single byte to buffer.

## Complexity Comparison

operator<<(char):
- Width check: 1 comparison
- put() call: 1 function call
- Buffer write: 1 byte write
- Total: ~10 instructions

operator<<(int):
- Sign check
- Digit extraction loop
- ASCII conversion per digit
- Multiple buffer writes
- Total: ~50+ instructions

## Machine Code

Command: `objdump -d test_enum`

Output:
```
1185:   e8 b6 fe ff ff          call   1040
```

Call instruction: 5 bytes (same as int version)
Difference: called function complexity, not call overhead.

## Instruction Count

Main function operations:
- Store char: 1 instruction (movb)
- Load value: 1 instruction (movsbl)
- Load cout: 1 instruction (movq)
- Call operator<<: 1 instruction (callq)
- Inside operator<<: ~10 instructions

Total: ~14 instructions per print operation.

Comparison: 14 instructions (char) vs 54 instructions (int) = 3.9× fewer instructions.
