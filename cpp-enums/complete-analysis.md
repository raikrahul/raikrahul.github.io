# C++ Enum Performance: Complete Assembly-Level Analysis

## AXIOM 1: Memory = Array of Bytes

Memory = array. Index = address. Value = byte. Byte = 8 bits. Bit ∈ {0,1}.

```
memory[0x1000] = 65
memory[0x1001] = 66
memory[0x1002] = 83
```

Address 0x1000 contains byte 65. Address 0x1001 contains byte 66. Address 0x1002 contains byte 83.

CPU operations: read(address) → byte. write(address, byte) → memory[address]=byte.

x86-64 virtual address space: 48 bits. Range: 0x0 → 0x7FFFFFFFFFFF.

## AXIOM 2: Integer = 4 Bytes

int = 4 bytes = 32 bits. Storage: little-endian (LSB first).

Value 0:
```
byte₀ = 0x00
byte₁ = 0x00  
byte₂ = 0x00
byte₃ = 0x00
```

Value 1:
```
byte₀ = 0x01
byte₁ = 0x00
byte₂ = 0x00
byte₃ = 0x00
```

Value 0x12345678:
```
memory[addr+0] = 0x78  (LSB)
memory[addr+1] = 0x56
memory[addr+2] = 0x34
memory[addr+3] = 0x12  (MSB)
```

sizeof(int) = 4. Verified: `sizeof(int)` in C++ returns 4 on x86-64.

## AXIOM 3: Character = 1 Byte

char = 1 byte = 8 bits. ASCII encoding: character → number.

```
'0' = 48 = 0x30 = 00110000₂
'1' = 49 = 0x31 = 00110001₂
'B' = 66 = 0x42 = 01000010₂
'S' = 83 = 0x53 = 01010011₂
```

sizeof(char) = 1. Verified: `sizeof(char)` returns 1.

Memory comparison: int uses 4×, char uses 1×.

## AXIOM 4: Assembly Instructions

x86-64 instruction set. Data movement instructions.

`movl` = move long = 4 bytes.
`movb` = move byte = 1 byte.
`$` = immediate value (constant).
`%rbp` = base pointer register.
`-N(%rbp)` = address (rbp - N).

Example 1:
```asm
movl $0, -8(%rbp)
```
Writes 4 bytes of value 0 to address (rbp-8).
memory[rbp-8] = 0x00
memory[rbp-7] = 0x00
memory[rbp-6] = 0x00
memory[rbp-5] = 0x00

Example 2:
```asm
movb $66, -9(%rbp)
```
Writes 1 byte of value 66 to address (rbp-9).
memory[rbp-9] = 0x42

Instruction encoding:
`movl` opcode = 0xC7 (REX prefix + ModR/M + displacement + immediate).
`movb` opcode = 0xC6 (ModR/M + displacement + immediate).

## AXIOM 5: Stack Layout

Stack = memory region. Stack pointer = %rsp. Base pointer = %rbp.

Stack grows downward: higher addresses → lower addresses.

Function prologue:
```asm
push %rbp           # save old base pointer
mov %rsp, %rbp      # set new base pointer
sub $16, %rsp       # allocate 16 bytes
```

Stack frame:
```
Higher addresses
    ↑
[return address]    ← pushed by call instruction
[saved %rbp]        ← %rbp points here (frame base)
[local var 1]       ← -4(%rbp)
[local var 2]       ← -8(%rbp)
[local var 3]       ← -12(%rbp)
[unused]            ← %rsp points here (frame top)
    ↓
Lower addresses
```

Local variables: negative offsets from %rbp.
-4(%rbp) = 4 bytes before base.
-8(%rbp) = 8 bytes before base.

## DERIVED 1: enum class Storage (from AXIOM 2)

Using AXIOM 2 (int = 4 bytes):

Source code:
```cpp
enum class OrderType { BUY, SELL };
```

Compiler assigns: BUY = 0, SELL = 1. Sequential integers starting from 0.

No explicit underlying type → defaults to int.

Storage: 4 bytes per enum value.

Verification:
```cpp
sizeof(OrderType) == 4  // ✓ measured
```

Declaration:
```cpp
OrderType type = OrderType::BUY;
```

Using AXIOM 4 (movl instruction) + AXIOM 5 (stack):

Generated assembly:
```asm
movl $0, -8(%rbp)
```

Stores 4-byte value 0 at stack location -8(%rbp).

Memory state:
```
Address      | Value
rbp-8        | 0x00
rbp-7        | 0x00
rbp-6        | 0x00
rbp-5        | 0x00
```

Total: 4 bytes occupied.

## DERIVED 2: Enum Names Disappear (from AXIOM 4)

Using AXIOM 4 (assembly instructions):

Assembly shows:
```asm
movl $0, -8(%rbp)
```

Instruction contains: opcode + operands (numeric only).
Instruction does NOT contain: string "BUY".

Compiler substitution at compile time:
```
OrderType::BUY  →  0
OrderType::SELL →  1
```

Binary verification:
```bash
$ strings test_enum | grep BUY
# no output
```

Objdump verification:
```bash
$ objdump -d test_enum | grep -A5 main
movl    $0, -8(%rbp)    # shows numeric 0, not "BUY"
```

Enum names exist in:
1. Source code (.cpp files)
2. Debug symbols (DWARF sections)
3. Compiler symbol table (compile-time only)

Enum names do NOT exist in:
1. Machine code (.text section)
2. Data section (.data, .rodata)
3. Runtime memory

Conclusion: names = compile-time only. Runtime = numeric values only.
