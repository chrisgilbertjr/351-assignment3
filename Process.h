
#ifndef _PROCESS_H_
#define _PROCESS_H_

/** Holds the process information */
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

    /**
     * Getter for Admitted
     */
    bool isAdmitted() const;

    /**
     * Getter for Terminated
     */
    bool isTerminated() const;
};

#endif
