#include "allocator.h"
#include <assert.h>
#include <stdio.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("  [%s] ", #name); \
        fflush(stdout); \
        tests_run++; \
        if (name()) { tests_passed++; printf("ok\n"); } \
        else        { printf("FAIL\n"); } \
    } while (0)

static bool test_alloc_returns_nonnull(void) {
    void *p = alloc(64);
    return p != NULL;
}

static bool test_alloc_zero_bytes(void) {
    void *p = alloc(0);
    return p != NULL;
}

static bool test_alloc_then_free(void) {
    void *p = alloc(128);
    if (!p) return false;
    return free_it(p);
}

static bool test_double_free_sets_errno(void) {
    void *p = alloc(64);
    if (!p) return false;
    if (!free_it(p)) return false;
    errno = 0;
    bool result = free_it(p);
    return result == false && errno == ErrorDoubleFree;
}

static bool test_alloc_reuses_freed_block(void) {
    void *a = alloc(256);
    void *b = alloc(256);
    if (!a || !b) return false;
    free_it(a);
    void *c = alloc(256);
    return c == a;
}

static bool test_three_allocs_distinct_addresses(void) {
    void *a = alloc(32);
    void *b = alloc(32);
    void *c = alloc(32);
    return a != b && b != c && a != c;
}

static bool test_alloc_after_free_in_middle(void) {
    void *a = alloc(100);
    void *b = alloc(100);
    void *c = alloc(100);
    if (!a || !b || !c) return false;
    free_it(b);
    void *d = alloc(50);
    return d != NULL;
}

static bool test_size_rounding(void) {
    void *a = alloc(1);
    void *b = alloc(1);
    if (!a || !b) return false;
    return (char *) b - (char *) a >= 4 + 4;
}

static bool test_many_allocs(void) {
    for (int i = 0; i < 1000; i++) {
        void *p = alloc(16);
        if (!p) return false;
    }
    return true;
}

static bool test_alloc_free_pattern(void) {
    void *ptrs[100];
    for (int i = 0; i < 100; i++) {
        ptrs[i] = alloc(64);
        if (!ptrs[i]) return false;
    }
    for (int i = 0; i < 100; i += 2) {
        if (!free_it(ptrs[i])) return false;
    }
    for (int i = 0; i < 50; i++) {
        void *p = alloc(32);
        if (!p) return false;
    }
    return true;
}

int main(void) {
    printf("Running tests...\n");

    TEST(test_alloc_returns_nonnull);
    TEST(test_alloc_zero_bytes);
    TEST(test_alloc_then_free);
    TEST(test_double_free_sets_errno);
    TEST(test_alloc_reuses_freed_block);
    TEST(test_three_allocs_distinct_addresses);
    TEST(test_alloc_after_free_in_middle);
    TEST(test_size_rounding);
    TEST(test_many_allocs);
    TEST(test_alloc_free_pattern);

    printf("\n%d/%d passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
