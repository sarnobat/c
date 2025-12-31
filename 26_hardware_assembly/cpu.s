// cpu_only.s — Apple Silicon ARM64 CPU example
// assemble & run:
//   clang -arch arm64 cpu_only.s -o cpu_only
//   ./cpu_only

    .global _main

_main:
    mov     x0, #1          // x0 = 1
    mov     x1, #1          // x1 = 1
loop:
    mul     x0, x0, x1      // x0 = x0 * x1
    add     x1, x1, #1      // x1++
    b       loop            // infinite loop