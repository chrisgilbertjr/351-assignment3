
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
void MemoryManager::InitializeProcessInfo(const char* filename)
{
    FILE* WorkloadFile = fopen(filename, "r");

    if (!WorkloadFile)
    {
        fprintf(stderr, "Error opening %s...\n", filename);

        NumProcesses = 0;
        Processes = 0;
        return;
    }

    char buffer[64] = {0};

    if (fgets(buffer, 64, WorkloadFile))
    {
        sscanf(buffer, "%u", &NumProcesses);
    }

    Processes = new Process[NumProcesses];

    for (int i = 0; i < NumProcesses; ++i)
    {
        if (fgets(buffer, 64, WorkloadFile))
        {
            sscanf(buffer, "%u", &Processes[i].Number);
        }

        if (fgets(buffer, 64, WorkloadFile))
        {
            sscanf(buffer, "%u %u", &Processes[i].ArrivalTime,
                                    &Processes[i].LifeTime);
        }

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
        fgets(buffer, 64, WorkloadFile);
    }
    gettimeofday(&Start, NULL);
    gettimeofday(&End, NULL);
}
//-----------------------------------------------------------------------------
void MemoryManager::Run()
{
}
//-----------------------------------------------------------------------------
unsigned long MemoryManager::GetCurrentMs()
{
    gettimeofday(&End, NULL);

    long int StartMs    = (Start.tv_sec * 1000) + (Start.tv_usec / 1000);
    long int EndMs      = (End.tv_sec * 1000) + (End.tv_usec / 1000);

    return EndMs - StartMs;
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
