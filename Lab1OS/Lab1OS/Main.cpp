#include "Allocator.h"
#include<conio.h>
//#include <locale.h>
#include <iostream>
#include <iomanip>
#include <cassert>


using namespace std;

int main()
{
    //wprintf(_T("привет !"));
    setlocale(LC_ALL, "Russian");
    //SetConsoleCP(1251);
    size_t size;
    cout << " Size in byte forfirst list of bloks  "; cin >> size;
    Allocator myAllocator(size);
    int option;
    cout << " Choose option  :" << endl;
    cout << "     1) Allocate block ;" << endl;
    cout << "     2) Reallocate block ;" << endl;
    cout << "     3) Delete block ;" << endl;
    cout << "     4) Print info ;" << endl;
    cout << "     5) End all ;" << endl;
    cin >> option;
    while (option != 5)
    {
        if (option == 1)
        {
            size_t tempSize;
            cout << "   --- Size in byte : "; cin >> tempSize;
            char* ptr = (char*)(myAllocator.mem_alloc(sizeof(char) * tempSize));
        }
        if (option == 2)
        {
            size_t tempSize;
            //char* ptr = (char*)(myAllocator.ListOfBlock());
            short block;
            cout << endl << "  - Choose block  "; cin >> block;
            void* searchedBlock = myAllocator.BlocByNum(block);

            while ((block < 1 || block > myAllocator.countBlocks() - 1 || (!GET_ALLOC((char*)searchedBlock - HEADER)) || (GET_SIZE((char*)searchedBlock - HEADER) == 0)) && !myAllocator.AllIsEmpty()) {
                cout << " Incorect number " << endl;
                cout << endl << "  - Choose block  : ";  cin >> block;
                searchedBlock = myAllocator.BlocByNum(block);
            }
            if (searchedBlock == nullptr) cout << " All blocks are free " << endl;
            else if (myAllocator.AllIsEmpty()) cout << " All blocks are free " << endl;
            else
            {
                cout << "  --- Size in byte :   "; cin >> tempSize;
                myAllocator.mem_realloc(searchedBlock, tempSize);
            }
        }
        if (option == 3)
        {
            //char* ptr = (char*)(myAllocator.ListOfBlock());
            //if (ptr == nullptr) cout << "   All blocks are free   " << endl;
            short block;
            cout << endl << "  - Choose block  "; cin >> block;
            void* searchedBlock = myAllocator.BlocByNum(block); while ((block < 1 || block > myAllocator.countBlocks() - 1 || (!GET_ALLOC((char*)searchedBlock - HEADER)) || (GET_SIZE((char*)searchedBlock - HEADER) == 0)) && !myAllocator.AllIsEmpty()) {
                cout << " Incorect number " << endl;
                cout << endl << "  - Choose block  : ";  cin >> block;
                searchedBlock = myAllocator.BlocByNum(block);
            }
            if (searchedBlock == nullptr) cout << " All blocks are free " << endl;
            else if (myAllocator.AllIsEmpty()) cout << " All blocks are free " << endl;
            else
                myAllocator.mem_free(searchedBlock);
        }
        if (option == 4)
        {
            myAllocator.mem_dump();
        }
        cout << endl << " Choose option  : "; cin >> option;
    }

    _getch();
    return 0;

}