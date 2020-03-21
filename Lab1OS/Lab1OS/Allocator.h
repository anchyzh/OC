#pragma once
#pragma once
#include <list>

#define WORD 4
#define HEADER 8
#define MIN_CHUNK_SIZE 128

#define PACK(size , alloc) (size | alloc) // Сойденяем

#define GET(pointer) (*(size_t*) (pointer))
#define PUT(pointer , value) ( *(size_t*) (pointer) = (value) )

#define GET_SIZE(bp) (GET(bp))
#define GET_SIZE_OF_PREV(bp) (GET(bp + WORD) & ~0x1)
#define GET_ALLOC(bp) (GET(bp + WORD) & 0x1)

#define HDRP(bp) ((char*)bp - HEADER)

#define NEXT_BLKP(bp) ((char*)bp + HEADER + GET_SIZE(bp))
#define PREV_BLKP(bp) ((char*)bp - HEADER - GET_SIZE_OF_PREV(bp))

#define GET_MEM_PTR(bp) ((void*)(bp + HEADER))
#define MAX(a , b) a < b ? b : a
#define CHUNK_SIZE(a) MAX(a,MIN_CHUNK_SIZE)
#define IS_TO_SMALL(a) size < (2 * HEADER + WORD)? true: false

#define ALINE_SIZE(size)  (size / WORD *WORD)< size ? ((size / WORD *WORD) + WORD):(size / WORD *WORD)

#define GET_COMBINRE_SIZE_WITH_PREV (currBlock) GET_SIZE(PREV_BLKP(currBlock)) + GET_SIZE(currBlock) + HEADER
#define GET_COMBINRE_SIZE_WITH_NEXT(currBlock) GET_SIZE(currBlock) + GET_SIZE(NEXT_BLKP(currBlock)) + HEADER
#define GET_COMBINE_ALL(currBlock)  GET_COMBINRE_SIZE_WITH_PREV(currBlock)+GET_SIZE(NEXT_BLKP(currBlock) + HEADER

class Allocator
{
public:
    Allocator(size_t inputChunkSize);
    ~Allocator();
    std::list<char*> chunks{};

    void* mem_alloc(size_t size);
    void mem_dump();
    void mem_free(void* addr);
    void* mem_realloc(void* addr, size_t size);


    char* addChunk(size_t minSize);
    void printInfo(char* block);
    void* BlocByNum(short num);

    char* blockMerge(char* currBlock);

    size_t getCombineSize(char* currBlock);
    int countBlocks();
    bool AllIsEmpty();

};