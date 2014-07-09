
#include "Process.h"

//-----------------------------------------------------------------------------
Process::Process()
    : TerminationTime(0)
    , AdmissionTime(0)
    , ArrivalTime(0)
    , LifeTime(0)
    , Number(0)
    , Size(0)
    , Admitted(false)
    , Terminated(false)
{
}
//-----------------------------------------------------------------------------
Process::~Process()
{
}
//-----------------------------------------------------------------------------
bool Process::isAdmitted() const
{
    return Admitted;
}
//-----------------------------------------------------------------------------
bool Process::isTerminated() const
{
    return Terminated;
}
//-----------------------------------------------------------------------------
