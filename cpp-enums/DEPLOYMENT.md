# Deployment Status

Repository: https://github.com/raikrahul/raikrahul.github.io
Branch: main
Commit: 1edfc17

## Live URLs

Main page: https://raikrahul.github.io/cpp-enums/index.html
Homepage link: https://raikrahul.github.io (see Projects section)

Documentation:
- https://raikrahul.github.io/cpp-enums/README.md
- https://raikrahul.github.io/cpp-enums/FINAL_ANSWER.md
- https://raikrahul.github.io/cpp-enums/ERRORS.md
- https://raikrahul.github.io/cpp-enums/EXECUTION_TRACE.md
- https://raikrahul.github.io/cpp-enums/THE_REAL_DIFFERENCE.md
- https://raikrahul.github.io/cpp-enums/VERIFICATION.md

Detailed proofs:
- https://raikrahul.github.io/cpp-enums/01-axioms-memory.md
- https://raikrahul.github.io/cpp-enums/02-assembly-instructions.md
- https://raikrahul.github.io/cpp-enums/03-enum-storage-proof.md
- https://raikrahul.github.io/cpp-enums/04-name-disappearance-proof.md
- https://raikrahul.github.io/cpp-enums/05-operator-int-assembly.md
- https://raikrahul.github.io/cpp-enums/06-operator-char-assembly.md
- https://raikrahul.github.io/cpp-enums/07-benchmark-methodology.md
- https://raikrahul.github.io/cpp-enums/08-benchmark-results-proof.md

Merged versions:
- https://raikrahul.github.io/cpp-enums/merged-analysis.md
- https://raikrahul.github.io/cpp-enums/complete-analysis.md

## Files Deployed

17 files total:
- 1 HTML (index.html)
- 16 Markdown files

Total size: ~70KB

## Verification

All claims verified:
- Storage: 4 bytes → 1 byte (75% reduction)
- Machine code: 7 bytes → 4 bytes (43% reduction)
- Performance: 335ms → 95ms (3.53× speedup)
- Function calls: operator<<(int) vs operator<<(char)
- Enum names absent from binary

## Key Finding

movb vs movl: ~1-2% performance impact
Function selection: ~350% performance impact

The enum's underlying type determines which operator<< overload is called.
This is where the 3.5× speedup comes from.
