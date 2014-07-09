/**
 * CS 351 OPERATING SYSTEMS - Section 1
 * 07/08/2014
 *
 * Group DBZ
 * - Chris Gilbert - Gilbertchris123@gmail.com
 * - Umair Tahir   - Umair007@csu.fullerton.edu
 * - Kourun Sok    - Kourun@csu.fullerton.edu
 *
 * Assignment #3 - Memory Management Simulator
 * Written in C++ on the Linux Platform
 *
 * Included files
 * - main.cpp
 * - MemoryManager.cpp
 * - Process.cpp
 * - Page.cpp
 * - MemoryManager.h
 * - Process.h
 * - Page.h
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MemoryManager.h"

int main()
{
    MemoryManager* memoryManager = new MemoryManager();
    unsigned int MemorySize = 0;
    unsigned int PageSize = 0;
    char filename[64] = {0};
    int  pageInput = 0;
    
    /** Get the memory size */
    std::cout << "Memory Size: ";
    std::cin  >> MemorySize;

    /** Get the page size */
    std::cout << "Page Size(1:100, 2:200, 3:400): ";
    std::cin  >> pageInput;

    switch (pageInput)
    {
        case 1:
            PageSize = 100;
            break;
        case 2:
            PageSize = 200;
            break;
        case 3:
            PageSize = 400;
            break;
        default:
            PageSize = 200;
    }

    /** Get the file name */
    std::cout << "Workload file name: ";
    std::cin  >> filename;

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
