# Part 08: Benchmark Results and Analysis

## Execution Results

Command: `./enum_benchmark`

### Run 1
```
Running benchmarks (no optimization)...

int enum (cast to int): 342 ms
int enum (to_char function): 103 ms
char enum (direct cast): 97 ms
```

### Run 2
```
Running benchmarks (no optimization)...

int enum (cast to int): 328 ms
int enum (to_char function): 109 ms
char enum (direct cast): 93 ms
```

### Run 3
```
Running benchmarks (no optimization)...

int enum (cast to int): 335 ms
int enum (to_char function): 106 ms
char enum (direct cast): 95 ms
```

## Statistical Analysis

### Mean Values
- int enum: (342 + 328 + 335) / 3 = 335 ms
- to_char: (103 + 109 + 106) / 3 = 106 ms
- char enum: (97 + 93 + 95) / 3 = 95 ms

### Standard Deviation
- int enum: σ = 5.7 ms
- to_char: σ = 2.5 ms
- char enum: σ = 1.6 ms

### Variance
- int enum: ±1.7% variation
- to_char: ±2.4% variation
- char enum: ±1.7% variation

Conclusion: Results consistent across runs.

## Per-Operation Cost

### Calculation
Total time / 10,000,000 iterations = time per operation

### Results
- int enum: 335 ms / 10,000,000 = 0.0000335 ms = 33.5 nanoseconds
- to_char: 106 ms / 10,000,000 = 0.0000106 ms = 10.6 nanoseconds
- char enum: 95 ms / 10,000,000 = 0.0000095 ms = 9.5 nanoseconds

## Speedup Analysis

### char enum vs int enum
335 ms / 95 ms = 3.53× faster

### to_char vs int enum
335 ms / 106 ms = 3.16× faster

### char enum vs to_char
106 ms / 95 ms = 1.12× faster

## Performance Ranking

1. **char enum (95 ms)** - Fastest ✓
   - 1-byte storage
   - Direct character output
   - No conversion overhead

2. **to_char function (106 ms)** - Middle
   - 4-byte storage
   - Function call overhead
   - Character output (simpler than int)

3. **int enum (335 ms)** - Slowest
   - 4-byte storage
   - Integer-to-string conversion
   - Multiple operations per print

## Bottleneck Analysis

### int enum bottleneck
- Integer-to-string conversion
- Division and modulo operations
- ASCII character generation
- Multiple buffer writes

### to_char bottleneck
- Function call overhead (~11 ms for 10M calls)
- Still faster than int conversion

### char enum advantage
- No conversion needed
- Single byte write
- Minimal instruction count

## Conclusion

char enum provides:
- 3.53× performance improvement over int enum
- 75% memory reduction (1 byte vs 4 bytes)
- Simpler assembly code
- Better cache efficiency

Verified by: Multiple benchmark runs, consistent timing, statistical analysis.
