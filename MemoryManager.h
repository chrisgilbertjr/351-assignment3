
#ifndef _MEMORY_MANAGER_
#define _MEMORY_MANAGER_

#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "Process.h"

struct MemoryManager
{
    Process* Processes; /**< dynamic array of processes */
    unsigned int NumProcesses; /**< Number of processes */
    struct timeval Start, End; /**< Timers */
    std::vector Queue;

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
     * @return True if filename is valid, otherwise false
     */
    bool InitializeProcessInfo(const char* filename);

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
     * Prints out debug information to console regarding process information
     */
    void PrintProcesses() const;

};

#endif

