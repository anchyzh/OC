#ifndef BUDDY_H_
#define BUDDY_H_

#include <stdint.h>

#define MAX_ORDER       10
#define MIN_ORDER       4   // 2 ** 4 == 16 bytes

/* диапазон порядка 0..MAX_ORDER, наибольшая блокировка 2 * *(MAX_ORDER) */
#define POOLSIZE        (1 << MAX_ORDER)

/* blocks are of size 2**i. */
#define BLOCKSIZE(i)    (1 << (i))

/* адрес buddy блока с  freelists[i]. */
#define _MEMBASE        ((uintptr_t)BUDDY->pool)
#define _OFFSET(b)      ((uintptr_t)b - _MEMBASE)
#define _BUDDYOF(b, i)  (_OFFSET(b) ^ (1 << (i)))
#define BUDDYOF(b, i)   ((pointer)( _BUDDYOF(b, i) + _MEMBASE))

// для выравнивания памяти более высокого порядка
#define ROUND4(x)       ((x % 4) ? (x / 4 + 1) * 4 : x)



typedef void* pointer; /* используется для нетипизированных указателей */

typedef struct buddy {
    pointer freelist[MAX_ORDER + 2];  // еще один слот для первого блока в пуле
    uint8_t pool[POOLSIZE];
} buddy_t;

pointer bmalloc(int size);
void bfree(pointer block);

void buddy_init();
void buddy_deinit();

void print_buddy();

#endif /* BUDDY_H_ */