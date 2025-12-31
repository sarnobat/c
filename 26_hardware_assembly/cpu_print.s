// cpu_print_working.s
// Build & run:
//   clang -arch arm64 cpu_print_working.s -o cpu_print_working
//   ./cpu_print_working

    .global _main
    .extern _write

    .text
_main:
    sub     sp, sp, #64        // buffer space
    mov     x20, #1            // counter = 1

loop:
    // convert number to ascii (naive conversion)
    mov     x0, x20            // x0 = number
    add     x1, sp, #32        // x1 = write pointer near end of buffer

itoa_loop:
    mov     x2, #10
    udiv    x3, x0, x2         // x3 = x0 / 10
    msub    x4, x3, x2, x0     // x4 = x0 - x3*10   → digit
    add     x4, x4, #'0'       // convert to ASCII
    strb    w4, [x1], #-1      // store & decrement pointer
    mov     x0, x3             // divide again
    cbnz    x0, itoa_loop

    add     x1, x1, #1         // x1 now points to 1st digit

    // append newline
    mov     w4, #'\n'
    strb    w4, [sp, #33]

    // write(fd=1, buf=x1, len=?)
    mov     x0, #1             // fd = stdout
    mov     x2, #1             // starting len = 1
    mov     x3, sp
len_calc:
    ldrb    w4, [x1, x2, lsl #0]
    cmp     w4, #'\n'
    beq     do_write
    add     x2, x2, #1
    b       len_calc

do_write:
    mov     x2, x2             // length already in x2
    bl      _write

    // next
    add     x20, x20, #1
    cmp     x20, #21
    b.lt    loop

    // exit
    mov     x0, #0
    mov     x16, #1
    svc     #0