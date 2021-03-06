#include "WorkerThread.h"

#include "JobSystem.h"
#include <Optick.h>
#include <sstream>

///-----------------------------------------------------------------------------
///! @brief Main function of this thread
///! @remark
///-----------------------------------------------------------------------------
int WorkerThread::WorkerFunction()
{
    std::stringstream str;
    str << "WorkerThread" << m_index;
    OPTICK_THREAD(str.str().c_str());

    while(isAlive())
    {
        if (m_threadPaused)
        {
            m_threadPaused = false;
            m_jobSystem->WorkerThreadActive(m_index);
        }
        auto& queue = m_jobSystem->GetJobQueue();
        auto workLoad = queue.GetNextWorkLoad();
        if (workLoad.m_job != nullptr)
        {
            //Execute this workload

            workLoad.m_job->Execute(m_index);
        }
        else
        {
            m_jobSystem->WorkerThreadSleeping(m_index);
            pauzeThread();
        }
    }

    return 0;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
void WorkerThread::pauzeThread()
{
    OPTICK_EVENT();

    m_threadPaused = true;
    //Probably need to sleep on a condition variable here that the jobsystem can set
    WaitForSingleObject(m_workAvailableHandle, INFINITE);
}
