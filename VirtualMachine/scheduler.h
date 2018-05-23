#pragma once

#include "dispatcher.h"
#include "mmu.h"
#include "pcb.h"
#include "readyqueue.h"
#include "safequeue.h"
#include "schedulingpolicy.h"
#include <string>
#include <queue>

#ifndef MAX_READY_QUEUE_SIZE
#define MAX_READY_QUEUE_SIZE 10
#endif

// TODO:: ** Scheduler

// ************************************************************
// Interface(s)
// ************************************************************

class IScheduler
{
public:
	IScheduler(SchedulingPolicy schedPolicy)
	{
		schedulingPolicy = schedPolicy;
	}

	virtual ~IScheduler() {}

	virtual void schedule() = 0;

protected:
	SchedulingPolicy schedulingPolicy;

private:
	// prevent copy-construction or assignment
	IScheduler(const IScheduler& other) {}
	IScheduler& operator=(const IScheduler& other) {}
};

// ************************************************************
// Implementation(s)
// ************************************************************

// Short-term (CPU) Scheduler...
// Selects from among the processes that are ready to execute 
// and allocates the CPU to one of them.

// Medium-term Scheduler...
// Sometimes it can be advantageous to remove a process from memory
// (and from active contention for the CPU) and thus reduce the degree of multiprogramming.
// Later, the process can be reintroduced into memory, 
// and its execution can be continued where it left off.

// Long-term (job) Scheduler...
// If more processes are submitted than can be executed immediately,
// then the processes are spooled to a mass-storage device (e.g. a disk).
// The long-term scheduler selects processes from this pool and
// loads them into memory for execution.

class JobScheduler : IScheduler
{
public:
	JobScheduler(
		MemoryManagementUnit *mem,
		SchedulingPolicy schedPolicy, 
		SafeQueue<ProcessControlBlock> *jq, 
		ReadyQueue *rq) :
		IScheduler(schedPolicy)
	{
		mmu = mem;
		jobQueue = jq;
		readyQueue = rq;
	}

	~JobScheduler()
	{
		if (mmu != NULL)
			mmu = NULL;

		if (jobQueue != NULL)
			jobQueue = NULL;

		if (readyQueue != NULL)
			readyQueue = NULL;
	}

	void schedule()
	{
		// selects a job from the job pool and creates it (i.e. puts it in the 'ready' queue)
		ProcessControlBlock selectedJob;

		if (jobQueue->empty())
		{
			// ERROR
			// job queue is empty
		}
		else
		{
			// TODO:: JobScheduler:: is ram full?
			if (readyQueue->size() < MAX_READY_QUEUE_SIZE)
			{
				// move process from "job queue" (i.e. disk) to "ready queue" (i.e. ram)

				// assuming queue is sorted by load time in ascending order (i.e job loaded first -> job loaded last)
				selectedJob = jobQueue->pop();

				selectedJob.status = ProcessStatus::READY;
				selectedJob.schedulingInfo.setArrivalTimeNow();

				mmu->removeProcess(MemoryType::DISK, selectedJob);
				bool processInserted = mmu->insertProcess(MemoryType::RAM, selectedJob);

				if (processInserted)
				{
					readyQueue->push(selectedJob);
				}
				else
				{
					// TODO:: JobScheduler:: failed to insert process
					// try compact
					//mmu->compactRAM();
					// try insert again
				}
			}
			else
			{
				// ready queue full; do nothing
			}
		}
	}

private:
	MemoryManagementUnit *mmu;
	SafeQueue<ProcessControlBlock> *jobQueue;
	ReadyQueue *readyQueue;
};

// ************************************************************
// ************************************************************
// ************************************************************

class CpuScheduler : IScheduler
{
public:
	CpuScheduler(
		MemoryManagementUnit *mem,
		SchedulingPolicy schedPolicy, 
		ReadyQueue *rq,
		Dispatcher *d) :
		IScheduler(schedPolicy)
	{
		mmu = mem;
		readyQueue = rq;
		dispatcher = d;
	}

	~CpuScheduler()
	{
		if (mmu != NULL)
			mmu = NULL;

		if (readyQueue != NULL)
			readyQueue = NULL;

		if (dispatcher != NULL)
			dispatcher = NULL;
	}

	void schedule()
	{
		// selects an available process (from a list of 'ready' jobs) 
		// for program execution on the CPU (i.e. dispatch)
		ProcessControlBlock selectedProcess;

		if (readyQueue->empty())
		{
			// ERROR
			// ready queue is empty
		}
		else
		{
			// IMPORTANT:: verify dispatcher can select process before selecting process to dispatch
			if (dispatcher->canSelectProcess())
			{
				selectedProcess = readyQueue->pop();
				dispatcher->selectProcessToDispatch(selectedProcess);
			}
			else
			{
				// dispatcher busy; do nothing
			}
		}
	}

private:
	MemoryManagementUnit *mmu;
	ReadyQueue *readyQueue;
	Dispatcher *dispatcher;
};
