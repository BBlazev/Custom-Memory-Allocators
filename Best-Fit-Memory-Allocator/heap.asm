bits 64
global memspace
%define Heapsize (1024 * 1024 * 1024 / 8)

Section .data
    memspace:
        dq _memspace    ; 64-bit pointer
        
Section .bss
    _memspace:
        resq Heapsize   ; reserve quadwords (64-bit)