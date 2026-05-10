CC      ?= gcc
NASM    ?= nasm
CFLAGS  ?= -Wall -Wextra -O2 -std=c11 -Iinclude
LDFLAGS ?=

POLICY ?= first_fit

ifeq ($(POLICY),first_fit)
  POLICY_DEFINE = -DPOLICY_FIRST_FIT
else ifeq ($(POLICY),best_fit)
  POLICY_DEFINE = -DPOLICY_BEST_FIT
else ifeq ($(POLICY),worst_fit)
  POLICY_DEFINE = -DPOLICY_WORST_FIT
else ifeq ($(POLICY),next_fit)
  POLICY_DEFINE = -DPOLICY_NEXT_FIT
else
  $(error POLICY must be one of: first_fit, best_fit, worst_fit, next_fit)
endif

BUILD_DIR = build/$(POLICY)

ALLOC_SRC = src/alloc.c src/find_block.c
ALLOC_OBJ = $(BUILD_DIR)/alloc.o $(BUILD_DIR)/find_block.o
HEAP_OBJ  = $(BUILD_DIR)/heap.o

LIB     = $(BUILD_DIR)/liballocator.a
TEST    = $(BUILD_DIR)/test_basic
DEMO    = $(BUILD_DIR)/demo

.PHONY: all test demo clean test-all

all: $(LIB) $(TEST) $(DEMO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(POLICY_DEFINE) -c $< -o $@

$(BUILD_DIR)/heap.o: src/heap.asm | $(BUILD_DIR)
	$(NASM) -f elf64 $< -o $@

$(LIB): $(ALLOC_OBJ) $(HEAP_OBJ)
	ar rcs $@ $^

$(TEST): tests/test_basic.c $(LIB)
	$(CC) $(CFLAGS) $(POLICY_DEFINE) $< -L$(BUILD_DIR) -lallocator -o $@

$(DEMO): examples/demo.c $(LIB)
	$(CC) $(CFLAGS) $(POLICY_DEFINE) $< -L$(BUILD_DIR) -lallocator -o $@

test: $(TEST)
	@echo "=== Testing $(POLICY) ==="
	@./$(TEST)

demo: $(DEMO)
	@./$(DEMO)

test-all:
	@for p in first_fit best_fit worst_fit next_fit; do \
		$(MAKE) --no-print-directory test POLICY=$$p || exit 1; \
	done

asan:
	$(MAKE) test POLICY=$(POLICY) \
		CFLAGS="-Wall -Wextra -O1 -g -std=c11 -Iinclude -fsanitize=address,undefined -fno-omit-frame-pointer" \
		LDFLAGS="-fsanitize=address,undefined"

clean:
	rm -rf build
