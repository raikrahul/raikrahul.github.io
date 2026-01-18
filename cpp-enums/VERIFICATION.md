# Verification Results

All claims in index.html verified on: 2026-01-18

## Storage Size Verification

```bash
$ clang++ -std=c++23 enum_storage.cpp -o enum_storage && ./enum_storage
sizeof(OrderType): 4
sizeof(OrderType2): 1
```

✓ Claim verified: int enum = 4 bytes, char enum = 1 byte

## Assembly Instruction Verification

```bash
$ clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum_verify.s
$ grep "movl.*\$1" test_enum_verify.s
movl    $1, -8(%rbp)

$ grep "movb.*\$83" test_enum_verify.s  
movb    $83, -50(%rbp)
```

✓ Claim verified: int enum uses movl, char enum uses movb with value 83 (ASCII 'S')

## Machine Code Size Verification

```bash
$ clang++ -std=c++23 -O0 test_enum.cpp -o test_enum_verify
$ objdump -d test_enum_verify | grep "movl.*\$0x1"
345f:   c7 45 f8 01 00 00 00    movl   $0x1,-0x8(%rbp)

$ objdump -d test_enum_verify | grep "movb.*\$0x53"
34c4:   c6 45 ce 53             movb   $0x53,-0x32(%rbp)
```

Byte count:
- movl: c7 45 f8 01 00 00 00 = 7 bytes
- movb: c6 45 ce 53 = 4 bytes

✓ Claim verified: movl = 7 bytes, movb = 4 bytes (43% reduction)

## Enum Name Disappearance Verification

```bash
$ strings test_enum_verify | grep BUY
(no output)

$ strings test_enum_verify | grep SELL
(no output)

$ nm test_enum_verify | grep -i buy
(no output)

$ nm test_enum_verify | grep -i sell
(no output)
```

✓ Claim verified: Enum names "BUY" and "SELL" do not exist in binary

## Benchmark Verification

```bash
$ clang++ -std=c++23 -O0 enum_benchmark.cpp -o enum_benchmark_verify
$ ./enum_benchmark_verify
Running benchmarks (no optimization)...

int enum (cast to int): 311 ms
int enum (to_char function): 100 ms
char enum (direct cast): 90 ms

$ ./enum_benchmark_verify
Running benchmarks (no optimization)...

int enum (cast to int): 324 ms
int enum (to_char function): 112 ms
char enum (direct cast): 87 ms

$ ./enum_benchmark_verify
Running benchmarks (no optimization)...

int enum (cast to int): 332 ms
int enum (to_char function): 102 ms
char enum (direct cast): 95 ms
```

Mean values:
- int enum: (311+324+332)/3 = 322ms
- to_char: (100+112+102)/3 = 105ms
- char enum: (90+87+95)/3 = 91ms

Speedup:
- char enum vs int enum: 322/91 = 3.54×
- to_char vs int enum: 322/105 = 3.07×
- char enum vs to_char: 105/91 = 1.15×

✓ Claim verified: char enum is 3.5× faster than int enum

## Summary

All claims in index.html verified:
- Storage: 4 bytes vs 1 byte (75% reduction)
- Machine code: 7 bytes vs 4 bytes (43% reduction)
- Performance: 3.5× speedup
- Enum names absent from binary
- Assembly uses movl vs movb
