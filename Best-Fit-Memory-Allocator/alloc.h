#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define public __attribute__((visibility("default"))) //this will be exported as library

#define private static
#define packed __attribute__((__packed__))
#define unused __attribute__((__unused__))

#define Maxwords ((1024*1024*1024/4)-1)

#define ErrorNoError    0
#define ErrorNoMemory   1
#define ErrorUnknown    2
#define ErrorDoubleFree 4

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef int32 word;

struct packed s_header{

    word w: 30;
    bool alloced: 1;
    bool unused reserved: 1;

};

typedef struct packed s_header header;

#define returnError(x) do{ \
    errno = (x);\
    return (void*) 0;\
} while(false)

#define findBlock(x) findBlock_((header*)memspace,(x),0)
#define show() show_((header*) memspace)

#define alloc_k(x) alloc((x) * 1024)
#define alloc_m(x) alloc((x) * (1024 * 1024))
#define alloc_g(x) alloc((x) * (1024 * 1024 * 1024))

public  bool free_it(void*);
private void show_(header*);

private header *findBlock_(header*, word, word);
private void *make_allocation(word,header*);
public  void *alloc(int32 bytes);

void zero(int8 *str, int16 size) {
    int8 *p;
    int16 counter;

    for (counter=0, p=str; counter<size; counter++, p++)
        *p = 0;

    return;
}

int main(int, char**);


