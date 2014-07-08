
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MemoryManager.h"

int main()
{
    MemoryManager* memoryManager = new MemoryManager();
    unsigned int MemorySize = 0;
    unsigned int PageSize = 0;

    // mem init here
    const char* filename = "in1.txt";
    MemorySize = 2000;
    PageSize = 100;

    /** Initialize the processes info with a file and test if successful */
    if (!memoryManager->InitializeProcessInfo(filename, MemorySize, PageSize))
    {
        return 1;
    }

    /** Run the memory manager */
    memoryManager->Run();
    
    /** Delete the Memory manager */
    delete memoryManager;

    /** Everything was a success */
    return 0;
}
