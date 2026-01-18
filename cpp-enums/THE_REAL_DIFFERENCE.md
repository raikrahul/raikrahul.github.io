# The Real Difference: It's Not Just movb vs movl

## Question
Is the entire performance difference just about `movb` vs `movl`?

## Answer
**NO.** The movb vs movl is just the storage/loading difference. The REAL performance difference is in **which operator<< function gets called**.

## Assembly Proof

### int case:
```asm
movl    $1, -8(%rbp)              # Store int (4 bytes)
movl    -8(%rbp), %esi            # Load int into register
movq    _ZSt4cout@GOTPCREL(%rip), %rdi
callq   _ZNSolsEi@PLT             # Call operator<<(int)
```

Demangled: `std::ostream::operator<<(int)`

### char case:
```asm
movb    $83, -5(%rbp)             # Store char (1 byte)
movsbl  -5(%rbp), %esi            # Load char into register
movq    _ZSt4cout@GOTPCREL(%rip), %rdi
callq   _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c@PLT
```

Demangled: `std::operator<<(std::ostream&, char)`

## Two Completely Different Functions

### operator<<(int) - Member Function
Location: `/usr/include/c++/13/ostream` line 191

```cpp
__ostream_type& operator<<(int __n);
```

Implementation calls `_M_insert()` which:
1. Calls `num_put` locale facet
2. Converts int to string:
   - Sign checking
   - Repeated division by 10
   - Modulo operations
   - ASCII conversion (digit + '0')
   - Multiple buffer writes
3. Complexity: O(log₁₀ n)
4. ~50 instructions

### operator<<(char) - Free Function
Location: `/usr/include/c++/13/ostream` line 570

```cpp
template<typename _Traits>
inline basic_ostream<char, _Traits>&
operator<<(basic_ostream<char, _Traits>& __out, char __c) {
  if (__out.width() != 0)
    return __ostream_insert(__out, &__c, 1);
  __out.put(__c);  // Just write the byte!
  return __out;
}
```

Implementation:
1. Width check (default=0, skips padding)
2. `put(__c)` writes single byte to buffer
3. Complexity: O(1)
4. ~10 instructions

## The Performance Breakdown

### Storage (movb vs movl):
- movl: 7 bytes machine code
- movb: 4 bytes machine code
- Difference: 3 bytes (43% reduction)
- Performance impact: **negligible** (both are single-cycle instructions)

### Function Call (operator<< implementation):
- operator<<(int): ~50 instructions (int-to-string conversion)
- operator<<(char): ~10 instructions (direct byte write)
- Difference: 40 instructions (80% reduction)
- Performance impact: **MASSIVE** (this is where 3.5× speedup comes from)

## Benchmark Verification

```
int enum:   335ms  (movl + operator<<(int) with conversion)
char enum:  95ms   (movb + operator<<(char) direct write)
Speedup:    3.53×
```

The 3.53× speedup is NOT from movb vs movl (that's maybe 1-2% difference).
The 3.53× speedup is from **avoiding int-to-string conversion**.

## Summary

```
movb vs movl:           ~1-2% performance difference
operator<<(char) vs 
operator<<(int):        ~350% performance difference (3.5×)
```

The movb vs movl is just a side effect. The real win is calling a completely different, much simpler function that doesn't need to convert numbers to strings.

## Verification Commands

```bash
# See which functions are called
clang++ -std=c++23 -S -O0 test_cout_int.cpp -o test_cout_int.s
grep "callq.*operator" test_cout_int.s

clang++ -std=c++23 -S -O0 test_cout_char.cpp -o test_cout_char.s  
grep "callq.*operator" test_cout_char.s

# Demangle the symbols
echo "_ZNSolsEi" | c++filt
# Output: std::ostream::operator<<(int)

echo "_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c" | c++filt
# Output: std::operator<<(std::ostream&, char)
```
