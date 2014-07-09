
#include <unistd.h>
#include "MemoryManager.h"
#include <iostream>

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
    bool     PrintTime = true; /** flag for printing time only once */
    long int CurrentMs = GetCurrentMs();
    unsigned int CurrentTime = 0; /**< The current time frame were in */
    unsigned int LastTime = 0;
    bool complete = false;
    bool terminated[NumProcesses];

    for (int i = 0; i < NumProcesses; ++i)
    {
        terminated[i] = false;
    }

    while (CurrentMs < MAX_MS)
    {
        CurrentTime = 0; 
        PrintTime   = true;
        
        /** Delete any memory that needs to be deleted */
        for (int i = 0; i < NumProcesses; ++i)
        {
            if (Processes[i].TerminationTime <= CurrentMs &&
               !Processes[i].isTerminated() &&
                Processes[i].isAdmitted())
            {
                if (PrintTime)
                {
                    CurrentTime = Processes[i].TerminationTime;
                    LastTime = CurrentTime;
                    printf("t = %4u:", CurrentTime);
                }

                PrintProcessCompleteion(Processes[i].Number, PrintTime);

                DeleteMemoryBlock(&Processes[i]);
                
                PrintMemoryMap();

                Processes[i].Terminated = true;
                terminated[i] = true;
                PrintTime = false;
            }
        }

        /** Processes are admitted to the process queue when needed here */
        for (int i = 0; i < NumProcesses; ++i)
        {
            /** Admit process to the process queue its arrival time is valid */
            if (!Processes[i].isAdmitted() &&
                !Processes[i].isTerminated() &&
                 Processes[i].ArrivalTime <= CurrentMs)
            {
                /** Only print the time once when necessary */
                if (PrintTime)
                {
                    CurrentTime = Processes[i].ArrivalTime;
                    LastTime = CurrentTime;
                    printf("t = %4u:", CurrentTime);
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
                Processes[i].TerminationTime = 999999;
                PrintTime = false;
            }
        }

        /** Map the memory if there is space available */
        if (Queue.size() > 0)
        {
                
            for (int i = 0; i < Queue.size(); ++i)
            {
                bool blockAvailable = CheckForFreeMemoryBlock(Queue[i].Size);

                if (blockAvailable)
                {
                    /** Print that the process is being moved to memory */
                    printf("           MM moves process %2u to memory\n", Queue[i].Number);

                    /** Map the memory */ 
                    MapMemoryBlock(&Queue[i]); 

                    /** Update the processes Admission and termination times */
                    int pIndex = Queue[i].Number-1;
                    Processes[pIndex].AdmissionTime = LastTime;
                    Processes[pIndex].TerminationTime = LastTime + Queue[i].LifeTime;

                    /** Delete the process from the queue */
                    Queue.erase(Queue.begin()+i);

                    /** Print the updated memory map */
                    PrintQueue();
                    PrintMemoryMap();
                }
            }
        }

        /** Print newline after printing a time cycle */
        if (!PrintTime)
        {
            printf("\n");
        }

        for (int i = 0; i < NumProcesses; ++i)
        {
            if (Processes[i].Terminated == false)
            {
                complete = false;
            }
        }

        if (complete)
        {
            break;
        }

        /** Update the time */
        CurrentMs = GetCurrentMs();
        complete = true;
    }

    float Total = 0.0f;
    float Turnaround = 0.0f;
    /** Calculate the turnaround time */
    for (int i = 0; i < NumProcesses; ++i)
    {
        Total += (float)(Processes[i].TerminationTime - Processes[i].ArrivalTime);
    }

    Turnaround = Total / (float)NumProcesses;

    printf("Turnaround Time: %0.2f (%0.1f/%u)\n\n", Turnaround, Total, NumProcesses);

    if (complete)
    {
        printf("All processes completed!\n");
    }
    else
    {
        printf("Timeout... 10000ms reached. Terminating...\n\n");
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
bool MemoryManager::CheckForFreeMemoryBlock(const unsigned int blockSize) const
{
    unsigned int Accumulate = 0;

    for (int i = 0; i < NumPages; ++i)
    {
        if (!Pages[i].isOccupied())
        {
            Accumulate += Pages[i].Size;
        }
    }

    if (Accumulate >= blockSize)
    {
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
void MemoryManager::DeleteMemoryBlock(Process* process)
{
    for (int i = 0; i < NumPages; ++i)
    {
        if (Pages[i].ProcessNumber == process->Number)
        {
            Pages[i].Occupied = false;
        }
    }
}
//-----------------------------------------------------------------------------
void MemoryManager::MapMemoryBlock(Process* process)
{
    unsigned int Accumulate = 0;
    unsigned int pageNumber = 1;

    for (int i = 0; i < NumPages; ++i)
    {
        if (!Pages[i].isOccupied())
        {
            Pages[i].ProcessNumber = process->Number;
            Pages[i].PageNumber = pageNumber;
            Pages[i].Occupied = true;
            pageNumber++;

            Accumulate += Pages[i].Size;
            
            if (Accumulate >= process->Size)
            {
                break;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void MemoryManager::PrintMemoryMap() const
{
    unsigned int accumulate = 0;
    unsigned int offset = 0;
    bool FirstPrint = true;

    printf("           Memory Map:");
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
                if (FirstPrint)
                {
                    printf(" %4u - %4u:", offset, offset+accumulate-1);
                    FirstPrint = false;
                }
                else
                {
                    printf("                       %4u - %4u:", offset, offset+accumulate-1);
                }
                printf(" Free frame(s)\n");

                offset = i * Pages[i].Size;
            }

            if (FirstPrint)
            {
                printf(" %4u - %4u:", offset, offset+Pages[i].Size-1);
                FirstPrint = false;
            }
            else
            {
                printf("                       %4u - %4u:", offset, offset+Pages[i].Size-1);
            }
            printf(" Process %2u, Page %2u\n", Pages[i].ProcessNumber,
                                               Pages[i].PageNumber);

            offset = (i+1) * Pages[i].Size;
            accumulate = 0;
        }
    }

    if (accumulate > 0)
    {
        if (FirstPrint)
        {
            printf(" %4u - %4u:", offset, offset+accumulate-1);
        }
        else
        {
            printf("                       %4u - %4u:", offset, offset+accumulate-1);
        }
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
void MemoryManager::PrintProcessCompleteion(const unsigned int processNumber, 
                                            bool time) const
{
    if (!time)
    {
        printf("         ");
    }

    printf("  Process %2u completes\n", processNumber);
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
