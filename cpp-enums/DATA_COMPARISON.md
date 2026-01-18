# Investigation vs Surprise: Raw Data

## Test 1: Storage Size

### Code
```cpp
enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };
OrderType type1 = OrderType::SELL;
OrderType2 type2 = OrderType2::SELL;
std::cout << sizeof(type1) << "\n";
std::cout << sizeof(type2) << "\n";
```

### Command
```bash
clang++ -std=c++23 enum_storage.cpp -o enum_storage && ./enum_storage
```

### Output
```
sizeof(OrderType): 4
sizeof(OrderType2): 1
```

### Assembly
```asm
# int enum
movl    $1, -8(%rbp)              # 7 bytes: c7 45 f8 01 00 00 00

# char enum  
movb    $83, -9(%rbp)             # 4 bytes: c6 45 f7 53
```

---

## Test 2: With cout (10M iterations)

### Code
```cpp
// int enum
for (int i = 0; i < 10000000; ++i) {
    OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
    null_stream << static_cast<int>(type);
}

// char enum
for (int i = 0; i < 10000000; ++i) {
    OrderType2 type = (i & 1) ? OrderType2::SELL : OrderType2::BUY;
    null_stream << static_cast<char>(type);
}
```

### Command
```bash
clang++ -std=c++23 -O0 enum_benchmark.cpp -o enum_benchmark
./enum_benchmark
```

### Output Run 1
```
int enum (cast to int): 342 ms
char enum (direct cast): 97 ms
```

### Output Run 2
```
int enum (cast to int): 328 ms
char enum (direct cast): 93 ms
```

### Output Run 3
```
int enum (cast to int): 335 ms
char enum (direct cast): 95 ms
```

### Assembly (int enum)
```asm
movl    -8(%rbp), %esi
movq    _ZSt4cout@GOTPCREL(%rip), %rdi
callq   _ZNSolsEi@PLT
```

### Assembly (char enum)
```asm
movsbl  -9(%rbp), %esi
movq    _ZSt4cout@GOTPCREL(%rip), %rdi
callq   _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c@PLT
```

### Function Names
```bash
$ echo "_ZNSolsEi" | c++filt
std::ostream::operator<<(int)

$ echo "_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c" | c++filt
std::operator<<(std::ostream&, char)
```

---

## Test 3: WITHOUT cout (100M iterations) - SURPRISE

### Code
```cpp
volatile int sink_int = 0;
volatile char sink_char = 0;

// int enum
for (int i = 0; i < 100000000; ++i) {
    OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
    sink_int = static_cast<int>(type);
}

// char enum
for (int i = 0; i < 100000000; ++i) {
    OrderType2 type = (i & 1) ? OrderType2::SELL : OrderType2::BUY;
    sink_char = static_cast<char>(type);
}
```

### Command
```bash
clang++ -std=c++23 -O0 pure_enum_test.cpp -o pure_enum_test
./pure_enum_test
```

### Output Run 1
```
int enum: 255 ms
char enum: 298 ms
```

### Output Run 2
```
int enum: 254 ms
char enum: 298 ms
```

### Output Run 3
```
int enum: 253 ms
char enum: 294 ms
```

### Output Run 4
```
int enum: 252 ms
char enum: 295 ms
```

### Assembly (int enum)
```asm
movl    -12(%rbp), %edx
andl    $1, %edx
xorl    %eax, %eax
movl    $1, %ecx
cmpl    $0, %edx
cmovnel %ecx, %eax
movl    %eax, -16(%rbp)
movl    -16(%rbp), %eax
movl    %eax, sink_int(%rip)
```

### Assembly (char enum)
```asm
movl    -12(%rbp), %ecx
andl    $1, %ecx
movb    $83, %al
movb    $66, %dl
movb    %dl, -42(%rbp)
cmpl    $0, %ecx
movb    %al, -41(%rbp)
jne     .LBB4_6
movb    -42(%rbp), %al
movb    %al, -41(%rbp)
.LBB4_6:
movb    -41(%rbp), %al
movb    %al, -13(%rbp)
movb    -13(%rbp), %al
movb    %al, sink_char(%rip)
```

### Instruction Verification
```bash
$ grep "cmovnel" pure_enum_test.s
cmovnel %ecx, %eax

$ grep "jne.*LBB4" pure_enum_test.s
jne     .LBB4_6

$ grep "Spill\|Reload" pure_enum_test.s | wc -l
4
```

---

## Side-by-Side Data

| Metric | int enum | char enum | Test 2 (cout) | Test 3 (no cout) |
|--------|----------|-----------|---------------|------------------|
| Storage | 4 bytes | 1 byte | - | - |
| Machine code | 7 bytes | 4 bytes | - | - |
| Time (10M cout) | 335ms | 95ms | char wins | - |
| Time (100M store) | 253ms | 296ms | - | int wins |
| Instruction | cmovnel | jne | - | - |
| Stack spills | 0 | 4 | - | - |
| Function called | operator<<(int) | operator<<(char) | Different | - |

---

## Raw Assembly Files

### int enum ternary
```asm
# From: type = (i & 1) ? SELL : BUY
movl    -12(%rbp), %edx          # edx = i
andl    $1, %edx                 # edx = i & 1
xorl    %eax, %eax               # eax = 0
movl    $1, %ecx                 # ecx = 1
cmpl    $0, %edx                 # compare edx with 0
cmovnel %ecx, %eax               # if edx != 0: eax = ecx
```

### char enum ternary
```asm
# From: type = (i & 1) ? SELL : BUY
movl    -12(%rbp), %ecx          # ecx = i
andl    $1, %ecx                 # ecx = i & 1
movb    $83, %al                 # al = 83
movb    $66, %dl                 # dl = 66
movb    %dl, -42(%rbp)           # stack[rbp-42] = 66
cmpl    $0, %ecx                 # compare ecx with 0
movb    %al, -41(%rbp)           # stack[rbp-41] = 83
jne     .LBB4_6                  # if ecx != 0: jump
movb    -42(%rbp), %al           # al = stack[rbp-42]
movb    %al, -41(%rbp)           # stack[rbp-41] = al
.LBB4_6:
movb    -41(%rbp), %al           # al = stack[rbp-41]
```

---

## Verification Commands

```bash
# Compile storage test
clang++ -std=c++23 enum_storage.cpp -o enum_storage

# Run storage test
./enum_storage

# Generate assembly
clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s

# View machine code
clang++ -std=c++23 -O0 test_enum.cpp -o test_enum
objdump -d test_enum | grep "movl.*\$1\|movb.*\$83"

# Compile cout benchmark
clang++ -std=c++23 -O0 enum_benchmark.cpp -o enum_benchmark

# Run cout benchmark
./enum_benchmark

# Compile pure storage benchmark
clang++ -std=c++23 -O0 pure_enum_test.cpp -o pure_enum_test

# Run pure storage benchmark
./pure_enum_test

# Check for conditional move
grep "cmovnel" pure_enum_test.s

# Check for branch
grep "jne" pure_enum_test.s

# Count stack operations
grep "Spill\|Reload" pure_enum_test.s | wc -l

# Demangle function names
echo "_ZNSolsEi" | c++filt
echo "_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c" | c++filt
```

---

## All Source Files

### enum_storage.cpp
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

### enum_benchmark.cpp
```cpp
#include <chrono>
#include <iostream>
#include <fstream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

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
    benchmark_int_enum();
    benchmark_char_enum();
    return 0;
}
```

### pure_enum_test.cpp
```cpp
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
    test_int_enum();
    test_char_enum();
    return 0;
}
```
