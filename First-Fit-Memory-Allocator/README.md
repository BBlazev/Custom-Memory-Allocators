# Custom Memory Allocator

A lightweight, educational memory allocator implementation in C with assembly-backed heap management.

## Overview

This project implements a custom memory allocation system similar to `malloc()` but with a specialized header format and memory management strategy. The allocator uses a 32-bit packed header structure to efficiently track memory blocks and their allocation status.

## Features

- **Custom Header Format**: 32-bit packed headers with 30-bit size field and allocation flags
- **Assembly-Backed Heap**: Heap space defined in assembly for precise memory control  
- **Recursive Block Finding**: Efficient block searching algorithm
- **Error Handling**: Built-in error codes and handling for memory exhaustion
- **Type Safety**: Custom type definitions for different word sizes

## Architecture

### Memory Layout
```
┌─────────────┬──────────────────────┐
│   Header    │    Allocated Data    │
│  (4 bytes)  │    (user memory)     │
└─────────────┴──────────────────────┘
```

### Header Structure
```c
struct s_header {
    word w: 30;           // Size in words (30 bits)
    bool alloced: 1;      // Allocation flag (1 bit)  
    bool reserved: 1;     // Reserved for future use (1 bit)
};
```

## Files

| File | Description |
|------|-------------|
| `alloc.h` | Header file with type definitions, structures, and function prototypes |
| `alloc.c` | Main allocator implementation with allocation and block-finding logic |
| `heap.asm` | Assembly file defining heap space |

## Building

### Prerequisites
- GCC compiler
- NASM assembler (for assembly file)
- Make (optional)

### Compilation
```bash
# Assemble the heap
nasm -f elf64 heap.asm -o heap.o

# Compile the allocator
gcc -c alloc.c -o alloc.o

# Link everything
gcc heap.o alloc.o -o allocator
```

### One-liner
```bash
nasm -f elf64 heap.asm -o heap.o && gcc heap.o alloc.c -o allocator
```

## Usage

### Basic Allocation
```c
#include "alloc.h"

int main() {
    // Allocate 100 bytes
    void *ptr = alloc(100);
    
    if (!ptr) {
        printf("Allocation failed: %d\n", errno);
        return -1;
    }
    
    // Use your memory...
    
    return 0;
}
```

### Finding Available Blocks
```c
// Find a block that can hold 500 words
header *block = findBlock(500);
if (!block) {
    printf("No suitable block found\n");
}
```

## Memory Management

### Allocation Process
1. Convert byte request to word count (4-byte alignment)
2. Search for available block using recursive `findBlock_()` 
3. Mark block as allocated and return pointer to user data (header + 4 bytes)

### Block Finding Algorithm
The allocator uses a recursive approach to traverse the heap:
- Check if current block is free and large enough
- If not, move to next block and recurse
- Return error if no suitable block found

## Configuration

### Memory Limits
```c
#define Maxwords ((1024*1024*1024/4)-1)  // ~1GB max allocation
```

### Error Codes
```c
#define ErrorNoMemory   1    // Insufficient memory
#define ErrorUnknown    2    // Unknown error
```

## Type System

The allocator includes convenient type definitions:
```c
typedef unsigned char int8;            // 8-bit unsigned
typedef unsigned short int int16;      // 16-bit unsigned  
typedef unsigned int int32;            // 32-bit unsigned
typedef unsigned long long int int64;  // 64-bit unsigned
typedef int32 word;                    // Basic word type
```

## Limitations

- **No Deallocation**: Currently implements allocation only (no `free()` equivalent)
- **Linear Search**: Block finding uses linear traversal (O(n) complexity)
- **No Coalescing**: Adjacent free blocks are not automatically merged
- **Fixed Heap Size**: 128MB heap size is compile-time constant

## Future Improvements

- [ ] Implement block deallocation (`free()` function)
- [ ] Add block coalescing for freed adjacent blocks  
- [ ] Implement free block linked list for faster allocation
- [ ] Add heap size configuration options
- [ ] Implement block splitting for better memory utilization
- [ ] Add memory debugging and leak detection


**Note**: This is an educational implementation and should not be used in production environments without significant testing and improvement.
