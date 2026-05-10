#include "allocator.h"

int main(void) {
    uint8_t *x, *x2, *x3, *x4;

    printf("Memspace: %p\n", memspace);

    x  = alloc(5);
    x2 = alloc(2000);
    x3 = alloc(1);
    (void) x; (void) x3;

    printf("\n--- After all allocations ---\n");
    show();

    free_it(x2);
    printf("\n--- After freeing x2 ---\n");
    show();

    x4 = alloc(1560);
    (void) x4;
    printf("\n--- After allocating x4 ---\n");
    show();

    return 0;
}
