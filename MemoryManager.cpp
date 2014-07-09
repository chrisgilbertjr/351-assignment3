
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

    /** Initialize the number of pages */
    NumPages = Size/pageSize;

    /** Dynamically create the array of processes */
    Processes = new Process[NumProcesses];

    /** Dynamically create the array of pages */
    Pages = new Page[NumPages];

    for (int i = 0; i < NumPages; ++i)
    {
        Pages[i].Size = pageSize;
    }

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

            Processes[i].Size = accumulate;
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
    bool     PrintTime = true;
    long int CurrentMs = GetCurrentMs();

    while (CurrentMs < MAX_MS)
    {
        PrintTime = true;
        for (int i = 0; i < NumProcesses; ++i)
        {
            if (!Processes[i].isAdmitted() &&
                 Processes[i].ArrivalTime <= CurrentMs)
            {
                /** Only print the time once when necessary */
                if (PrintTime)
                {
                    printf("t = %4u:", Processes[i].ArrivalTime);
                }

                /** Print that the process arrived */
                PrintProcessArrival(Processes[i].Number, PrintTime);

                /** Add the process to the queue */
                Queue.push_back(Processes[i]);

                /** Print the queue and memory map */
                PrintQueue();
                PrintMemoryMap();

                /** The process has been admitted and the time was printed */
                Processes[i].Admitted = true;
                PrintTime = false;
            }
        }

        /** Map the memory if there is space available */
        if (Queue.size() > 0)
        {
            bool blockAvailable = FreeMemoryBlock(Queue[0].Size);
                
            while(blockAvailable)
            {
                /** Map the memory */ 
                MapMemory(&Queue[0]); 
                Queue.erase(Queue.begin());
                blockAvailable = false;

                if (Queue.size() > 0)
                {
                    blockAvailable = FreeMemoryBlock(Queue[0].Size);
                }

                printf("           MM moves process %2u to memory\n", Queue[0].Number);
                PrintMemoryMap();
            }
        }

        /** Print newline after printing a time cycle */
        if (!PrintTime)
        {
            printf("\n");
        }

        /** Update the time */
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
bool MemoryManager::FreeMemoryBlock(const unsigned int blockSize) const
{
    unsigned int MaxAccumulate = 0;
    unsigned int Accumulate = 0;

    for (int i = 0; i < NumPages; ++i)
    {
        if (!Pages[i].isOccupied())
        {
            Accumulate += Pages[i].Size;
        }
        else
        {
            if (Accumulate >= MaxAccumulate)
            {
                MaxAccumulate = Accumulate;
            }

            Accumulate = 0;
        }
    }

    if (Accumulate >= MaxAccumulate)
    {
        MaxAccumulate = Accumulate;
    }

    if (MaxAccumulate >= blockSize)
    {
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
void MemoryManager::MapMemory(Process* process)
{
    unsigned int Accumulate = 0;
    unsigned int Index = 0;
    bool map = false;

    for (int i = 0; i < NumPages; ++i)
    {
        if (Pages[i].isOccupied())
        {
            Accumulate = 0;
            Index = i+1;
        }
        else
        {
            Accumulate += Pages[i].Size;

            if (Accumulate >= process->Size)
            {
                map = true;
                break;
            }

        }
    }

    if (map)
    {
        unsigned int pageIndex = 0;
        unsigned int loops = process->Size/Pages[Index].Size;
        for (int i = 0; i < loops; ++i)
        {
            pageIndex = Index + i;

            Pages[pageIndex].ProcessNumber = process->Number;
            Pages[pageIndex].PageNumber = i+1;
            Pages[pageIndex].Occupied = true;
        }
    }
    else
    {
        printf("ERROR MAPPING MEMORY!");
    }
}
//-----------------------------------------------------------------------------
void MemoryManager::PrintMemoryMap() const
{
    unsigned int accumulate = 0;
    unsigned int offset = 0;

    printf("           Memory Map:\n");
    for (int i = 0; i < NumPages; ++i)
    {
        if (!Pages[i].isOccupied())
        {
            accumulate += Pages[i].Size;
        }
        else
        {
            if (accumulate > 0)
            {
                printf("                %4u - %4u:", offset, offset+accumulate-1);
                printf(" Free frame(s)\n");
            }

            printf("                %4u - %4u:", offset, offset+Pages[i].Size-1);
            printf(" Process %2u, Page %2u\n", Pages[i].ProcessNumber,
                                               Pages[i].PageNumber);

            offset = (i+1) * Pages[i].Size;
            accumulate = 0;
        }
    }

    if (accumulate > 0)
    {
        printf("                %4u - %4u:", offset, offset+accumulate-1);
        printf(" Free frame(s)\n");
    }
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
    printf("           Input Queue: [");
    for (int i = 0; i < Queue.size(); ++i)
    {
        printf("%u", Queue[i].Number);
        
        if (i+1 != Queue.size())
        {
            printf(" ");
        }
    }
    printf("]\n");
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
        printf("Memory Space   : %u\n", Processes[i].Size);
        printf("\n");
    }
}
//-----------------------------------------------------------------------------
