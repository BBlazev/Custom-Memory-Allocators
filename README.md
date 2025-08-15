# Custom Memory Allocators

A comprehensive implementation of four fundamental memory allocation strategies in C, demonstrating the core algorithms that power dynamic memory management in operating systems and runtime libraries.

## Overview

This project implements four classic memory allocation algorithms from scratch, providing working examples of how dynamic memory allocation works under the hood. Each implementation demonstrates different trade-offs between allocation speed, memory efficiency, and fragmentation patterns.

## What Are Memory Allocators?

Memory allocators manage dynamic memory allocation requests from programs, deciding where to place new allocations within a heap of available memory. The choice of allocation strategy significantly impacts performance, memory utilization, and fragmentation characteristics.

When you call `malloc()` in C or `new` in C++, you're using a memory allocator. This project recreates that functionality with four different strategies:

- **First-Fit**: Allocates the first suitable block found
- **Best-Fit**: Allocates the smallest suitable block to minimize waste
- **Worst-Fit**: Allocates the largest suitable block to maximize remaining fragment size
- **Next-Fit**: Continues searching from where the last allocation ended

## Implementation Architecture

### Core Components

- **Header Structure**: 4-byte packed headers containing block size and allocation status
- **Heap Management**: 1GB memory space with automatic expansion
- **Block Traversal**: Linked structure through pointer arithmetic
- **Memory Safety**: Bounds checking and error handling

### Technical Specifications

- **Language**: C (C17 standard)
- **Heap Size**: 1 GB maximum (configurable)
- **Word Size**: 32-bit words (4 bytes)
- **Alignment**: Automatic word-boundary alignment
- **Platform**: Cross-platform (Windows, Linux, macOS)

### Memory Layout

```
[Header][User Data][Header][User Data]...[End Marker]
   4B    Variable     4B    Variable        4B (w=0)
```

Each allocated block consists of:
- **Header**: Contains block size in words and allocation status
- **User Data**: The memory space returned to the caller
- **Alignment**: All blocks aligned to 4-byte boundaries

## Allocation Strategies Comparison

| Strategy   | Time Complexity | Memory Efficiency | Fragmentation Pattern | Use Case |
|------------|----------------|-------------------|----------------------|----------|
| First-Fit  | O(n/2) average | Moderate          | Clustered at start   | General purpose |
| Best-Fit   | O(n) average    | High              | Many small fragments | Memory-constrained |
| Worst-Fit  | O(n) average    | Low               | Large fragments      | Varied allocation sizes |
| Next-Fit   | ~O(n/2) average   | Moderate          | Distributed          | Long-running processes |

## Performance Characteristics

### First-Fit
- **Advantages**: Fast allocation, simple implementation, good cache locality
- **Disadvantages**: Fragmentation clustering, potential waste at heap start

### Best-Fit
- **Advantages**: Minimal memory waste, efficient space utilization
- **Disadvantages**: Slower allocation, creates many unusable small fragments

### Worst-Fit
- **Advantages**: Large remaining fragments may be useful for future allocations
- **Disadvantages**: High immediate waste, generally poor performance

### Next-Fit
- **Advantages**: Better fragmentation distribution, prevents start-of-heap clustering
- **Disadvantages**: May skip suitable blocks, slightly more complex state management

## Build Instructions

### Prerequisites
- GCC or Clang compiler with C17 support
- NASM assembler (for heap.asm)
- Make or CMake build system

## API Reference

### Core Functions

```c
void* alloc(int32 bytes)
```
Allocates memory block of specified size. Returns pointer to user data area or NULL on failure.

```c
bool free_it(void* address)
```
Frees previously allocated memory block. Returns true on success, false on error.

```c
void show()
```
Displays current heap state showing all allocated and free blocks.

### Memory Management

```c
void zero(int8* str, int16 size)
```
Zeros out memory region for security and debugging.

## Testing and Benchmarking

Each implementation includes tests demonstrating:
- Basic allocation and deallocation
- Fragmentation behavior under various patterns  
- Edge cases and error conditions
- Performance characteristics with different workloads

## Educational Value

This project demonstrates fundamental concepts in:
- **Systems Programming**: Low-level memory management, pointer arithmetic
- **Algorithm Design**: Trade-offs between time and space complexity
- **Data Structures**: Linked structures, memory layout optimization  
- **Performance Engineering**: Cache behavior, fragmentation analysis
- **Software Engineering**: Modular design, comprehensive testing

## Real-World Applications

The algorithms implemented here form the foundation of:
- Operating system memory managers
- Language runtime heap managers (malloc, new)
- Embedded system memory allocators

**Note**: These implementations are for educational and demonstration purposes. For production use, consider proven allocators like jemalloc, tcmalloc, or system malloc implementations.
