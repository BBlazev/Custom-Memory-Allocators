# Custom Memory Allocators

Four memory allocator search policies — first-fit, next-fit, best-fit, worst-fit — implemented on top of a shared implicit free-list design in C, with the heap backed by a static 1 GiB region declared in NASM.

This is a learning project. It is not a drop-in `malloc` replacement. See [Limitations](#limitations).

## Design

All four allocators share the same memory layout:

```
[hdr][user data][hdr][user data] ... [hdr w=0]
 4B     4·w B     4B     4·w B          4B
```

- **Header** (`struct s_header`, 4 bytes, packed): 30-bit block size in 4-byte words, 1 alloced bit, 1 reserved bit.
- **Heap** (`memspace`, NASM): static 1 GiB region in `.bss`. Fixed size — no `sbrk`, no `mmap`, no expansion.
- **End sentinel**: a header with `w == 0` marks the unused tail of the heap. Allocation walks until it finds a fitting free block or the sentinel.
- **Alignment**: all blocks are 4-byte aligned. Requests are rounded up: `words = ceil(bytes / 4)`.

The four implementations differ only in `findBlock_`, the function that picks which free block to use.

| Allocator   | Search policy                                                  |
| ----------- | -------------------------------------------------------------- |
| First-Fit   | First free block with `w >= requested`                         |
| Next-Fit    | Same, but resumes from the last allocation site (rover)        |
| Best-Fit    | Free block with smallest `w >= requested` (full scan)          |
| Worst-Fit   | Free block with largest `w >= requested` (full scan)           |

## API

```c
void  *alloc(int32 bytes);   // returns user pointer, or NULL with errno set
bool   free_it(void *addr);  // marks block free, zeros user region
void   show();               // prints the heap as a list of blocks
```

Convenience macros: `alloc_k(n)`, `alloc_m(n)`, `alloc_g(n)` for KiB/MiB/GiB requests.

`errno` values on failure: `ErrorNoMemory`, `ErrorDoubleFree`, `ErrorUnknown`.

## Build

Each strategy lives in its own folder. Inside one of them:

```sh
nasm -f elf64 heap.asm -o heap.o
gcc -c main.c -o main.o
gcc main.o heap.o -o allocator
./allocator
```

Linux x86-64 only. The NASM file uses the SysV calling convention and ELF symbol naming.

## Example

```c
int8 *a = alloc(5);      // 5 bytes -> 2 words
int8 *b = alloc(2000);   // 2000 bytes -> 500 words
free_it(b);
int8 *c = alloc(1560);   // first/best/worst will reuse b's slot; next-fit may not
show();
```


## License

Apache-2.0.
