#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "buddy.h"

buddy_t* BUDDY = 0;

void buddy_init(buddy_t* buddy) {

    BUDDY = buddy;
    memset(BUDDY, 0, sizeof(buddy_t));
    BUDDY->freelist[MAX_ORDER] = BUDDY->pool;
}

void test001() {
    void* p1, * p2;
    p1 = bmalloc(4);
    p2 = bmalloc(13);

    bfree(p2);
    bfree(p1);

    print_buddy();
}

void test002() {
    void* p1, * p2;
    p1 = bmalloc(4);
    p2 = bmalloc(13);

    bfree(p1);
    bfree(p2);

    print_buddy();
}

void test003() {
    void* p1, * p2, * p3;
    p1 = bmalloc(25);
    p2 = bmalloc(9);
    p3 = bmalloc(7);

    bfree(p2);
    bfree(p3);
    bfree(p1);

    print_buddy();
}

int main() {

    buddy_t* buddy = (buddy_t*)malloc(sizeof(buddy_t));
    buddy_init(buddy);
    test001();
    test002();
    test003();
    buddy_deinit();
    free(buddy);
    return 0;
}


pointer bmalloc(int size) {

    int i, order;
    pointer block, buddy;

    // calculate minimal order for this size
    i = 0;
    while (BLOCKSIZE(i) < size + 1) // one more byte for storing order
        i++;

    order = i = (i < MIN_ORDER) ? MIN_ORDER : i;

    // level up until non-null list found
    for (;; i++) {
        if (i > MAX_ORDER)
            return NULL;
        if (BUDDY->freelist[i])
            break;
    }

    // remove the block out of list
    block = BUDDY->freelist[i];
    BUDDY->freelist[i] = *(pointer*)BUDDY->freelist[i];

    // split until i == order
    while (i-- > order) {
        buddy = BUDDYOF(block, i);
        BUDDY->freelist[i] = buddy;
    }

    // store order in previous byte
    *(((uint8_t*)block - 1)) = order;
    return block;
}

void bfree(pointer block) {

    int i;
    pointer buddy;
    pointer* p;

    // fetch order in previous byte
    i = *(((uint8_t*)block - 1));

    for (;; i++) {
        // calculate buddy
        buddy = BUDDYOF(block, i);
        p = &(BUDDY->freelist[i]);

        // find buddy in list
        while ((*p != NULL) && (*p != buddy))
            p = (pointer*)*p;

        // not found, insert into list
        if (*p != buddy) {
            *(pointer*)block = BUDDY->freelist[i];
            BUDDY->freelist[i] = block;
            return;
        }
        // found, merged block starts from the lower one
        block = (block < buddy) ? block : buddy;
        // remove buddy out of list
        *p = *(pointer*)*p;
    }
}



void buddy_deinit() {

    BUDDY = 0;
}

/*
 * The following functions are for simple tests.
 */

static int count_blocks(int i) {

    int count = 0;
    pointer* p = &(BUDDY->freelist[i]);

    while (*p != NULL) {
        count++;
        p = (pointer*)*p;
    }
    return count;
}

static int total_free() {

    int i, bytecount = 0;

    for (i = 0; i <= MAX_ORDER; i++) {
        bytecount += count_blocks(i) * BLOCKSIZE(i);
    }
    return bytecount;
}

static void print_list(int i) {

    printf("freelist[%d]: \n", i);

    pointer* p = &BUDDY->freelist[i];
    while (*p != NULL) {
        printf("    0x%08lx, 0x%08lx\n", (uintptr_t)*p, (uintptr_t)*p - (uintptr_t)BUDDY->pool);
        p = (pointer*)*p;
    }
}

void print_buddy() {

    int i;

    printf("========================================\n");
    printf("MEMPOOL size: %d\n", POOLSIZE);
    printf("MEMPOOL start @ 0x%08x\n", (unsigned int)(uintptr_t)BUDDY->pool);
    printf("total free: %d\n", total_free());

    for (i = 0; i <= MAX_ORDER; i++) {
        print_list(i);
    }
}