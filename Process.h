
#ifndef _PROCESS_H_
#define _PROCESS_H_

struct Process
{
    unsigned int Number;
    unsigned int ArrivalTime;
    unsigned int LifeTime;
    unsigned int AdmissionTime;
    unsigned int TerminationTime;
    unsigned int Size;
    bool Admitted;
    bool Terminated;

    Process();

    ~Process();

    bool isAdmitted() const;

    bool isTerminated() const;
};

#endif
