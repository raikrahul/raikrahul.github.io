# Errors in Understanding

## Error 1: movb vs movl is the performance difference
Line: Initial question "is this entire thing all about movb vs mov"
What went wrong: Assumed instruction size = performance impact
What should be: movb vs movl = 1-2% impact, function selection = 350% impact
Why sloppy: Focused on visible assembly, ignored function calls
What missed: Different operator<< functions called
How to prevent: Always demangle function names, check what's inside PLT calls

## Error 2: Enum executes at runtime
Line: "how does the system know that ::BUY means B"
What went wrong: Thought enum names stored in binary
What should be: Compiler substitutes names with values at compile time
Why sloppy: Confused compile-time with runtime
What missed: strings/nm show no enum names in binary
How to prevent: Run strings/nm on binary, check what actually exists

## Error 3: static_cast has runtime cost
Line: "static cast is a function call"
What went wrong: Thought cast = function call = stack operations
What should be: static_cast = compile-time type change, zero runtime cost
Why sloppy: Saw assembly, assumed cast generated code
What missed: Cast happens before assembly generation
How to prevent: Compare assembly with/without cast, count instructions

## Error 4: Storage size = performance
Line: Assumed 1 byte vs 4 bytes = 3.5× speedup
What went wrong: Conflated memory size with execution speed
What should be: Storage affects cache/memory, not instruction count
Why sloppy: Logical leap without measurement
What missed: Actual instruction execution inside operator<<
How to prevent: Profile actual execution, measure instruction counts

## Error 5: movb vs movl explains benchmark results
Line: "so tell me this, the .S is same?"
What went wrong: Thought same assembly = same performance
What should be: Same caller assembly, different callee implementation
Why sloppy: Only looked at main(), ignored library functions
What missed: operator<<(int) vs operator<<(char) implementations
How to prevent: Disassemble library functions, read libstdc++ source

## Error 6: Enum makes no difference
Line: "so this means what? enum made no difference at all?"
What went wrong: Concluded enum irrelevant after finding function difference
What should be: Enum's underlying type determines which function selected
Why sloppy: Separated cause (enum type) from effect (function selection)
What missed: Compile-time overload resolution based on type
How to prevent: Trace compiler decision process, understand overload resolution

## What movb vs movl Actually Does

movb: Move Byte (8-bit)
- Opcode: c6
- Immediate: 1 byte
- Total: 4 bytes machine code
- Stores: 1 byte on stack
- Loads: 1 byte from stack (movsbl sign-extends to 32-bit register)

movl: Move Long (32-bit)
- Opcode: c7
- Immediate: 4 bytes
- Total: 7 bytes machine code
- Stores: 4 bytes on stack
- Loads: 4 bytes from stack

Performance impact:
- Both: Single-cycle instructions
- Difference: Code size (3 bytes), memory footprint (3 bytes)
- Execution: Negligible (~1-2%)

## Actual Performance Chain

1. Enum declaration: `: char` vs `: int` (compile-time)
2. Storage: movb vs movl (runtime, negligible)
3. Type: char vs int (compile-time)
4. Overload resolution: operator<<(char) vs operator<<(int) (compile-time)
5. Function execution: 10 instructions vs 50 instructions (runtime, 350% difference)

## Measurements

Storage instruction size:
- movl: 7 bytes (verified: c7 45 f8 01 00 00 00)
- movb: 4 bytes (verified: c6 45 ce 53)
- Difference: 3 bytes

Function calls:
- int: _ZNSolsEi (operator<<(int))
- char: _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c (operator<<(char))
- Verified: grep "callq" assembly output

Execution time:
- int: 335ms (10M iterations)
- char: 95ms (10M iterations)
- Speedup: 3.53×

Instruction count:
- operator<<(int): ~54 instructions
- operator<<(char): ~14 instructions
- Difference: 40 instructions (74% reduction)

## Conclusion

movb vs movl: Storage mechanism
Function selection: Performance mechanism
Enum underlying type: Determines both
