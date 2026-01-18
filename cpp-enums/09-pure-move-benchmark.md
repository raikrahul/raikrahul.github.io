

# Pure Move Benchmark: The "Natural Width" Surprise

While `std::cout` shows a massive 3.3x speedup for `char` enums due to avoiding conversion logic, the picture changes when we isolate **raw memory movement**.

## The Benchmark Code

We strip away all I/O and conversion logic. This tests purely the CPU's ability to move data from register to memory (`volatile` is used to force memory writes).

`pure_enum_test.cpp`:
```cpp
#include <chrono>
#include <iostream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

volatile int sink_int = 0;
volatile char sink_char = 0;

void test_int_enum() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 100 Million iterations
    for (int i = 0; i < 100000000; ++i) {
        OrderType type = (i & 1) ? OrderType::SELL : OrderType::BUY;
        sink_int = static_cast<int>(type);
    }
    
    // ... timing code ...
}

void test_char_enum() {
    // ... same structure ...
    sink_char = static_cast<char>(type);
}
```

## Results: Optimized (-O3)

```text
int enum:  33 ms
char enum: 25 ms
Speedup:   ~1.3x
```

**Derivation:**
1. `int` write = 4 bytes to L1 Cache.
2. `char` write = 1 byte to L1 Cache.
3. 1 byte < 4 bytes.
4. Less cache pressure = Slight performance gain.

## Results: Unoptimized (-O0)

```text
int enum:  254 ms
char enum: 297 ms
Result:    char is ~17% SLOWER
```

**The Axiomatic Surprise:**
Why is the "smaller" data type slower?

### 1. Natural Word Width
The x86-64 CPU is natively a 64-bit machine. Its internal highways are 64 bits wide. 32-bit (`int`) is also a "natural" supported width for legacy reasons.
accessing 8-bit (`char`) registers (like `al`) requires "partial register access".

### 2. The Assembly Trace (-O0)

**Int Path (Fast path in O0):**
```asm
movl    $1, -8(%rbp)        # Store 1 (32-bit)
movl    -8(%rbp), %eax      # Load 1 (32-bit)
movl    %eax, sink_int(%rip)# Write to memory (32-bit)
```
*   Clean 32-bit moves.

**Char Path (Slow path in O0):**
```asm
movb    $83, -9(%rbp)       # Store 'S' (8-bit)
movsbl  -9(%rbp), %eax      # Move Sign Extension Byte to Long (8->32 expansion!)
movb    %al, sink_char(%rip)# Write low byte (8-bit)
```
*   **Axiom:** The compiler promotes `char` to `int` for register operations because registers are 32/64 bit.
*   **Cost:** `movsbl` (Sign Extension) adds overhead.
*   **Cost:** Working with `%al` (partial register) can incur penalties depending on CPU generation.

## Conclusion

1.  **With I/O (`cout`)**: `char` is **3.3x FASTER** because it avoids the `int->text` conversion algorithm.
2.  **Raw Moves (Optimized)**: `char` is **1.3x FASTER** because of reduced memory bandwidth.
3.  **Raw Moves (Debug)**: `char` is **SLOWER** because the CPU naturally prefers 32/64-bit integers and requires sign-extension instructions (`movsbl`) to handle single bytes in unoptimized streams.

**Core Lesson:** `enum class : char` is better for *storage* and *I/O*, but `int` is the CPU's "native language". Use `char` when you have lots of them (storage) or print them often (I/O).
