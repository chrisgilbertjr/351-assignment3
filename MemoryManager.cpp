
#include <unistd.h>
#include "MemoryManager.h"

//-----------------------------------------------------------------------------
MemoryManager::MemoryManager()
    : NumProcesses(0)
    , Processes(0)
{
}
//-----------------------------------------------------------------------------
MemoryManager::~MemoryManager()
{
    delete [] Processes;
}
//-----------------------------------------------------------------------------
bool MemoryManager::InitializeProcessInfo(const char*        filename,
                                          const unsigned int memorySize,
                                          const unsigned int pageSize)
{
    /** open the file */
    FILE* WorkloadFile = fopen(filename, "r");

    /** check if the file opened successfully */
    if (!WorkloadFile)
    {
        fprintf(stderr, "Error opening %s...\n", filename);

        NumProcesses = 0;
        Processes = 0;
        return false;
    }

    /** Buffer used for FILE I/O */
    char buffer[64] = {0};

    /** Get the number of processes */
    if (fgets(buffer, 64, WorkloadFile))
    {
        sscanf(buffer, "%u", &NumProcesses);
    }

    /** Initialize the size of managed memory */
    Size = memorySize;

    /** Dynamically create the array of processes */
    Processes = new Process[NumProcesses];

    /** Dynamically create the array of pages */
    Pages = new Page[Size/pageSize];

    for (int i = 0; i < NumProcesses; ++i)
    {
        /** Get the process Number */
        if (fgets(buffer, 64, WorkloadFile))
        {
            sscanf(buffer, "%u", &Processes[i].Number);
        }

        /** Get the arrival time and lifetime of the process */
        if (fgets(buffer, 64, WorkloadFile))
        {
            sscanf(buffer, "%u %u", &Processes[i].ArrivalTime,
                                    &Processes[i].LifeTime);
        }

        /** Get the total memory space the process occupies */
        if (fgets(buffer, 64, WorkloadFile))
        {
            char* token;
            unsigned int Blocks = 0;
            unsigned int accumulate = 0;
            sscanf(buffer, "%u", &Blocks);

            token = strtok(buffer, " ");

            for (int j = 0; j < Blocks; ++j)
            {
                token = strtok(NULL, " ");
                accumulate += atoi(token);
            }

            Processes[i].MemorySpace = accumulate;
        }

        /** Read the blank line between the processes */
        fgets(buffer, 64, WorkloadFile);
    }

    /** Initialize timers */
    gettimeofday(&Start, NULL);
    gettimeofday(&End, NULL);

    return true;
}
//-----------------------------------------------------------------------------
void MemoryManager::Run()
{
    long int CurrentMs = GetCurrentMs();
    bool PrintTime = true;

    while (CurrentMs < MAX_MS)
    {
        PrintTime = true;
        for (int i = 0; i < NumProcesses; ++i)
        {
            if (!Processes[i].isAdmitted() &&
                 Processes[i].ArrivalTime <= CurrentMs)
            {
                if (PrintTime)
                {
                    printf("t = %4u:", Processes[i].ArrivalTime);
                }

                PrintProcessArrival(Processes[i].Number, PrintTime);
                Queue.push_back(&Processes[i]);
                Processes[i].Admitted = true;
                PrintTime = false;
            }
        }
        //PrintQueue();

        CurrentMs = GetCurrentMs();
    }
}
//-----------------------------------------------------------------------------
long int MemoryManager::GetCurrentMs()
{
    /** update current time */
    gettimeofday(&End, NULL);

    /** convert timers to Ms */
    long int StartMs    = (Start.tv_sec * 1000) + (Start.tv_usec / 1000);
    long int EndMs      = (End.tv_sec * 1000) + (End.tv_usec / 1000);

    /** Return total Ms since initialization */
    return EndMs - StartMs;
}
//-----------------------------------------------------------------------------
void MemoryManager::PrintProcessArrival(const unsigned int processNumber,
                                        bool time) const
{
    if (!time)
    {
        printf("         ");
    }

    printf("  Process %2d arrives\n", processNumber);
}
//-----------------------------------------------------------------------------
void MemoryManager::PrintQueue() const
{
    printf("\n\n ");
    for (int i = 0; i < Queue.size(); ++i)
    {
        printf("%u ", Queue[i]->Number);
    }
    printf("\n\n");
}
//-----------------------------------------------------------------------------
void MemoryManager::PrintProcesses() const
{
    printf("\nTotal Processes: %u", NumProcesses);
    printf("\n===================\n");
    printf("\n");
    for (int i = 0; i < NumProcesses; ++i)
    {
        printf("Process Number : %u\n", Processes[i].Number);
        printf("Arrival Time   : %u\n", Processes[i].ArrivalTime);
        printf("Total LifeTime : %u\n", Processes[i].LifeTime);
        printf("Memory Space   : %u\n", Processes[i].MemorySpace);
        printf("\n");
    }
}
//-----------------------------------------------------------------------------
