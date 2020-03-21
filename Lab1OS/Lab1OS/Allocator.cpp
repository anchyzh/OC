#include "Allocator.h"
#include <iostream>
#include <iomanip>
#include <cassert>


using namespace std;

Allocator::Allocator(size_t size)
{
    if (IS_TO_SMALL(size))
    {
        cout << "Too small adress space.It will be 128 byte" << endl;
        addChunk(128);
    }
    addChunk(ALINE_SIZE(size));
}


Allocator::~Allocator()
{
    for (char* chunk : chunks)
        delete chunk;
}

char* Allocator::addChunk(size_t Size)
{
    Size = CHUNK_SIZE(Size);
    char* newChunk = new char[Size];
    chunks.push_back(newChunk);
    char* start = newChunk;
    PUT(start, Size - 2 * HEADER);
    PUT(start + WORD, 0);
    //��������� ����� �����
    PUT(NEXT_BLKP(start), 0);
    PUT(NEXT_BLKP(start) + WORD, PACK(Size - 2 * HEADER, 1));
    return newChunk;
}

void Allocator::printInfo(char* block)
{
    cout << endl << "   ____________________________ " << endl
        << "   | size : " << GET_SIZE(block) << endl
        << "   | size of previous block : " << GET_SIZE_OF_PREV(block) << endl
        << "   | available : " << GET_ALLOC(block) << endl
        << "   |_______________________________ " << endl;
}

void* Allocator::mem_alloc(size_t size)
{
    size = ALINE_SIZE(size);
    char* cur = nullptr;

    for (char* currChunk : chunks) //������� ���������� ����
    {
        cur = currChunk;

        while ((GET_SIZE(cur) != 0) &&  //���� ��� �� ����� �����
            (GET_ALLOC(cur) ||  //���� �� �����
            (GET_SIZE(cur) < size))) //�����  ������ ��� ��� ����
            cur = NEXT_BLKP(cur);  //���������� ������

        if (GET_SIZE(cur) != 0) break; //���� ����� �� ����� ������ = �� ����� ���
    }

    if ((GET_SIZE(cur - HEADER) == 0) || (GET_SIZE(cur) == 0))
    {
        char* newChunk = addChunk(size + 2 * HEADER); //������� ����� ���� ������ ��� �� ����� ����
        cur = newChunk;
    }

    if (GET_SIZE(cur) >= size + HEADER + WORD) //���� �� ����� ���� � ������� ����������� ������
    {
        size_t newSize = GET_SIZE(cur) - size - HEADER; //������� ������ ������ �����
        PUT(cur, size);

        char* newBlock = NEXT_BLKP(cur);
        PUT(newBlock, newSize);
        PUT(newBlock + WORD, PACK(size, 0));
        PUT(NEXT_BLKP(newBlock) + WORD, PACK(newSize, GET_ALLOC(NEXT_BLKP(newBlock))));
    }

    PUT(cur + WORD, PACK(GET_SIZE_OF_PREV(cur), 1));

    return GET_MEM_PTR(cur);
}




void Allocator::mem_dump()
{
    cout << "________________________________________________________ " << endl
        << "|    Information about allocator                       | " << endl << "|  - Min size of chunk  = " << MIN_CHUNK_SIZE << "                     |" << endl
        << "|  - Number of chunk   = " << chunks.size() << "                        |" << endl
        << "|______________________________________________________| " << endl;


    unsigned int blockNum = 1;
    unsigned int chunkNum = 1;

    for (char* currChunk : chunks)
    {
        cout << endl << "        * Chunk  " << chunkNum << endl;

        unsigned int AllocatedMem = 0;
        unsigned int FreeMem = 0;
        char* currBlock = currChunk;

        while (true)
        {    //��������� ���������� ��� ����
            cout << endl << " > Block � " << blockNum << " : ";
            printInfo(currBlock);

            if (GET_ALLOC(currBlock))
                AllocatedMem += GET_SIZE(currBlock);
            else
                FreeMem += GET_SIZE(currBlock);
            blockNum++;

            if (GET_SIZE(currBlock) != 0) currBlock = NEXT_BLKP(currBlock);
            else break;
        }
        //��������� ��������� ��� ����
        cout << endl << " Info about list of blocks: " << endl
            << "  All memory = " << AllocatedMem << endl
            << "  Free memory = " << FreeMem << endl;

        chunkNum++;
    }
}


char* Allocator::blockMerge(char* currBlock)
{
    char* next = NEXT_BLKP(currBlock);
    char* prev = PREV_BLKP(currBlock);
    bool nextAlloc = GET_ALLOC(next);
    bool prevAlloc = GET_ALLOC(prev);
    prevAlloc = GET_SIZE_OF_PREV(currBlock) == 0 ? true : GET_ALLOC(prev);

    if (prev != nullptr && !prevAlloc && next != nullptr && !nextAlloc)
    {
        // ��������� ���������� ������� � ��������� �����
        PUT(prev, GET_SIZE(prev) + GET_SIZE(currBlock) + GET_SIZE(next) + 2 * HEADER);
        PUT(NEXT_BLKP(next) + WORD, PACK(GET_SIZE(prev), GET_ALLOC(NEXT_BLKP(next))));
        return prev; //���������� ������ �����������
    }
    else if (prev != nullptr && !prevAlloc)
    {
        // ��������� ���������� � ������� �����
        PUT(prev, GET_SIZE(prev) + GET_SIZE(currBlock) + HEADER);
        PUT(next + WORD, PACK(GET_SIZE(prev), GET_ALLOC(next)));
        return prev; //���������� ������ �����������
    }
    else if (next != nullptr && !nextAlloc)
    {
        // ��������� ������� � ��������� �����
        PUT(currBlock, GET_SIZE(currBlock) + GET_SIZE(next) + HEADER);
        PUT(NEXT_BLKP(next) + WORD, PACK(GET_SIZE(prev), GET_ALLOC(NEXT_BLKP(next))));
        return currBlock; //���������� ������ �������� �����
    }

    return currBlock;
}



void* Allocator::mem_realloc(void* addr, size_t size)
{
    if (addr == nullptr)
        return mem_alloc(size);

    size = ALINE_SIZE(size);

    char* curr = (char*)addr - HEADER;
    int mergedBlocksSize = getCombineSize(curr);

    if (mergedBlocksSize >= size + HEADER + WORD) //���� ���� ������ ��� ���� = ��������� �� 2 �����
    {
        char* newCurrBlock = blockMerge(curr);
        PUT(newCurrBlock + WORD, PACK(GET_SIZE_OF_PREV(newCurrBlock), 1));

        size_t newBlockSize = GET_SIZE(newCurrBlock) - size - HEADER;
        PUT(newCurrBlock, size);
        size_t amountToCopy = size < GET_SIZE(curr) ? size : GET_SIZE(curr);

        if (newCurrBlock != curr)
        {
            memmove(GET_MEM_PTR(newCurrBlock), GET_MEM_PTR(curr), amountToCopy);
        }

        char* newNextBlock = NEXT_BLKP(newCurrBlock);

        PUT(newNextBlock, newBlockSize);
        PUT(newNextBlock + WORD, PACK(size, 0));
        PUT(NEXT_BLKP(newNextBlock) + WORD, PACK(newBlockSize, GET_ALLOC(NEXT_BLKP(newNextBlock))));
        return GET_MEM_PTR(newCurrBlock);
    }
    else if (mergedBlocksSize < size) //���� ����� ������ ��� ����� ������ ������
    {

        void* newCurrBlock = mem_alloc(size);
        if (newCurrBlock == nullptr)
            return nullptr;

        memmove(newCurrBlock, GET_MEM_PTR(curr), size);
        mem_free(addr);
        return newCurrBlock;
    }

    //���� ������ ������������ ������ ����� ��� ��� �����
    char* newCurrBlock = blockMerge(curr);
    PUT(newCurrBlock + WORD, PACK(GET_SIZE_OF_PREV(newCurrBlock), 1));

    size_t amountToCopy = size < GET_SIZE(curr) ? size : GET_SIZE(curr);
    if (newCurrBlock != curr)
        memmove(GET_MEM_PTR(newCurrBlock), GET_MEM_PTR(curr), amountToCopy);

    return GET_MEM_PTR(newCurrBlock);
}


size_t Allocator::getCombineSize(char* currBlock)
{
    char* next = NEXT_BLKP(currBlock);
    char* prev = PREV_BLKP(currBlock);
    bool nextAlloc = GET_ALLOC(next);
    bool prevAlloc = GET_ALLOC(prev);
    prevAlloc = GET_SIZE_OF_PREV(currBlock) == 0 ? true : GET_ALLOC(prev);

    if (prev != nullptr && !prevAlloc && next != nullptr && !nextAlloc)
        return (GET_SIZE(prev) + GET_SIZE(currBlock) + GET_SIZE(next) + 2 * HEADER); // ��������� ���������� ������� � ��������� �����


    if (prev != nullptr && !prevAlloc)
        return (GET_SIZE(prev) + GET_SIZE(currBlock) + HEADER); // ��������� ���������� �������  �����

    if (next != nullptr && !nextAlloc)
        return (GET_SIZE(currBlock) + GET_SIZE(next) + HEADER); // ��������� ������� � ��������� �����

    return GET_SIZE(currBlock);
}

void Allocator::mem_free(void* addr)
{
    char* curr = (char*)addr - HEADER;
    PUT(curr + WORD, PACK(GET_SIZE_OF_PREV(curr), 0));
    blockMerge(curr);
}


void* Allocator::BlocByNum(short num) {
    unsigned int blockNum = 0;
    for (char* currChunk : chunks)
    {
        char* currBlock = currChunk;
        while (true) {
            blockNum++;
            if (blockNum == num) { return (void*)(currBlock + HEADER); }
            if (GET_SIZE(currBlock) != 0) currBlock = NEXT_BLKP(currBlock);
            else break;
        }
    }

    return nullptr;
}

int Allocator::countBlocks()
{
    unsigned int count = 0;

    for (char* currChunk : chunks)
    {
        char* currBlock = currChunk;
        while (true) {
            count++;
            if (GET_SIZE(currBlock) != 0) currBlock = NEXT_BLKP(currBlock); //���� ����� �� �����
            else break;
        }
    }
    return count;
}

bool Allocator::AllIsEmpty()
{

    for (char* currChunk : chunks)
    {
        char* currBlock = currChunk;
        while (true) {
            if (GET_ALLOC(currBlock) && GET_SIZE(currBlock) != 0) {
                return false;
            }
            if (GET_SIZE(currBlock) != 0) currBlock = NEXT_BLKP(currBlock); //���� ����� �� �����
            else break;
        }
        return true;
    }
}