# C++ Enum Performance Analysis

Complete analysis of `enum class : char` vs `enum class : int` performance.

## Start Here

**[index.html](index.html)** - Main publication (open in browser)

## Quick Answer

**Q: Is the 3.5× speedup just from movb vs movl?**

**A: NO.** 
- movb vs movl: ~1-2% difference
- Function selection: ~350% difference

The enum's underlying type determines which `operator<<` overload is called:
- `int` → `operator<<(int)` → int-to-string conversion (~50 instructions)
- `char` → `operator<<(char)` → direct byte write (~10 instructions)

See **[FINAL_ANSWER.md](FINAL_ANSWER.md)** for complete explanation.

## Documentation Files

### Core Analysis
- **[index.html](index.html)** - Complete analysis (23KB, open in browser)
- **[FINAL_ANSWER.md](FINAL_ANSWER.md)** - What the enum actually does (5.3KB)
- **[THE_REAL_DIFFERENCE.md](THE_REAL_DIFFERENCE.md)** - movb vs function selection (3.5KB)
- **[EXECUTION_TRACE.md](EXECUTION_TRACE.md)** - Step-by-step trace with line numbers (9.3KB)

### Verification
- **[VERIFICATION.md](VERIFICATION.md)** - All claims verified with commands (2.6KB)

### Detailed Proofs
- **[01-axioms-memory.md](01-axioms-memory.md)** - Memory fundamentals
- **[02-assembly-instructions.md](02-assembly-instructions.md)** - movl vs movb analysis
- **[03-enum-storage-proof.md](03-enum-storage-proof.md)** - sizeof verification
- **[04-name-disappearance-proof.md](04-name-disappearance-proof.md)** - strings/nm proof
- **[05-operator-int-assembly.md](05-operator-int-assembly.md)** - operator<<(int) analysis
- **[06-operator-char-assembly.md](06-operator-char-assembly.md)** - operator<<(char) analysis
- **[07-benchmark-methodology.md](07-benchmark-methodology.md)** - Benchmark design
- **[08-benchmark-results-proof.md](08-benchmark-results-proof.md)** - Statistical analysis

### Merged Versions
- **[merged-analysis.md](merged-analysis.md)** - Dense format, all proofs (8.5KB)
- **[complete-analysis.md](complete-analysis.md)** - Full detailed version (4.2KB)

## Key Results

| Metric | int enum | char enum | Improvement |
|--------|----------|-----------|-------------|
| Storage | 4 bytes | 1 byte | 75% reduction |
| Instruction size | 7 bytes | 4 bytes | 43% reduction |
| Instructions executed | ~54 | ~14 | 3.9× fewer |
| Execution time | 335ms | 95ms | 3.53× faster |
| Function called | operator<<(int) | operator<<(char) | Different |

## Reproduction

```bash
# Storage verification
clang++ -std=c++23 enum_storage.cpp -o enum_storage
./enum_storage

# Assembly verification
clang++ -std=c++23 -S -O0 test_enum.cpp -o test_enum.s
grep "movl\|movb" test_enum.s

# Machine code verification
clang++ -std=c++23 -O0 test_enum.cpp -o test_enum
objdump -d test_enum | grep "movl\|movb"

# Function call verification
clang++ -std=c++23 -S -O0 verify_functions.cpp -o verify_functions.s
grep "callq" verify_functions.s | grep operator

# Benchmark
clang++ -std=c++23 -O0 enum_benchmark.cpp -o enum_benchmark
./enum_benchmark
```

## Environment

- OS: x86-64 Linux
- Compiler: Clang 18.1.3
- Standard Library: libstdc++ 13.3.0
- Optimization: -O0 (no optimization)
- Standard: C++23

## The Bottom Line

The enum's underlying type (`: char`) is a compile-time directive that:
1. Reduces memory footprint (4 bytes → 1 byte)
2. Reduces code size (7 bytes → 4 bytes)
3. **Changes which operator<< is called (this is the 3.5× win)**

The speedup is NOT from movb vs movl.
The speedup is from avoiding int-to-string conversion.

## License

All measurements and analysis are factual observations of compiler behavior.
No proprietary code included. All source code is original.
