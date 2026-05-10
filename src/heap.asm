bits 64
global memspace
%define Heapsize (1024 * 1024 * 1024 / 8)

section .data
    memspace:
        dq _memspace

section .bss
    _memspace:
        resq Heapsize

section .note.GNU-stack noalloc noexec nowrite progbits
