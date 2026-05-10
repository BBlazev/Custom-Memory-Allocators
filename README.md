## Custom Memory Allocators

Four memory allocator search policies — first-fit, next-fit, best-fit, worst-fit — sharing one implicit free-list implementation. Heap is a static 1 GiB BSS region declared in NASM. Linux x86-64.

This is a learning project. Not a `malloc` replacement.

### Layout

```
include/allocator.h     shared types, macros, prototypes
src/alloc.c             alloc, free_it, show, make_allocation
src/find_block.c        all four policies, selected at compile time
src/heap.asm            1 GiB static heap
tests/test_basic.c      assertion-based tests
examples/demo.c         the original printf walkthrough
```

### Build

```
make POLICY=first_fit       # default
make POLICY=best_fit
make POLICY=worst_fit
make POLICY=next_fit

make test POLICY=best_fit
make test-all
make asan POLICY=next_fit
make demo POLICY=first_fit
make clean
```

### API

```c
void *alloc(uint32_t bytes);
bool  free_it(void *address);
void  show(void);
```

`errno` on failure: `ErrorNoMemory`, `ErrorDoubleFree`, `ErrorUnknown`.

### Status

Done:
- Iterative search.
- Four distinct policies behind one shared codebase.
- Real test suite under sanitizers.

Next:
- Block splitting on alloc.
- Boundary tags + immediate coalescing on free.
- Explicit free list, then segregated free lists.
- Bump, pool, buddy.
- Per-thread arenas + benchmark harness.
