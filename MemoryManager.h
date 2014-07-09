
#ifndef _MEMORY_MANAGER_
#define _MEMORY_MANAGER_

#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "Process.h"
#include "Page.h"

/* Max running time */
#define MAX_MS 10000

struct MemoryManager
{
    std::vector<Process> Queue;         /**< FCFS Process queue */
    struct timeval       Start;        /**< Base timer */
    struct timeval       End;          /**< Current timer */
    unsigned int         NumProcesses; /**< Number of processes */
    unsigned int         NumPages;     /**< Number of pages */
    unsigned int         Size;         /**< Size of memory being managed */
    Process*             Processes;    /**< Dynamic array of processes */
    Page*                Pages;        /**< Dynamic array of pages */

    /**
     * Constructor
     */
    MemoryManager();

    /**
     * Destructor
     */
    ~MemoryManager();

    /**
     * Initializes the processes used in the Memory manager
     *
     * @param filename The name of the file that contains the process info
     * @param memorySize The total size of managed memory kb
     * @param pageSize The size of each page in memory in kb
     * @return True if filename is valid, otherwise false
     */
    bool InitializeProcessInfo(const char*        filename, 
                               const unsigned int memorySize, 
                               const unsigned int pageSize);

    /**
     * Runs the simulation of the Memory manager
     */
    void Run();

    /**
     * Gets the total time in ms since InitializeProcessInfo has been called
     *
     * @return The time since in ms since InitializeProcessInfo was last called
     */
    long int GetCurrentMs();

    /**
     * Checks if there is a free chunk of memory with size blockSize
     *
     * @param blockSize The size of the memory block 
     * @return True if there is free memory of size blockSize, otherwise false
     */
    bool CheckForFreeMemoryBlock(const unsigned int blockSize) const;

    void DeleteMemoryBlock(Process* process);

    void MapMemoryBlock(Process* process);

    /**
     * Prints the memory map
     */
    void PrintMemoryMap() const;

    /**
     * Prints that a process has arrived.
     * Formatting depends on if the time was printed on this line as well.
     *
     * @param processNumber The process number to print
     * @param time Format flag depending on if time was printed this line
     */
    void PrintProcessArrival(const unsigned int processNumber, bool time) const;

    /**
     * Prints that a process has completed.
     * Farmatting depends on if the time was printed on this line as well.
     */
    void PrintProcessCompleteion(const unsigned int processNumber, bool time) const;

    /**
     * Prints the input queue of processes.
     */
    void PrintQueue() const;

    /**
     * Prints out debug information to console regarding process information
     */
    void PrintProcesses() const;
};

#endif

