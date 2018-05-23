#pragma once

#include "mmu.h"
#include "pcb.h"
#include "processor.h"
#include "readyqueue.h"
#include "timeduration.h"
#include <vector>

/*
	places the selected job or program in an 'execution context' to run on the virtual CPU.
	(i.e. gives control of the CPU to the process selected by the short-term scheduler)

	assigns a process (that is in RAM) to the CPU.
	responsible for context switching of jobs

	extract parameter data from the PCB
	and accordingly set the CPU's PC, and other registers,
	before the OS calls the CPU to execute the job.

	- switching context
	- switching to user mode
	- jumping to the proper location in the user program to restart that program

	sets up user registers
	changes process status (e.g. from 'ready' to 'running')

	invoked during every process switch
*/
class Dispatcher
{
public:
	const bool PREEMPTING_ENABLED;

	Dispatcher(
		MemoryManagementUnit *mem, 
		std::vector<Processor> *cpus, 
		ReadyQueue *rq, 
		bool enablePreempting) :
		PREEMPTING_ENABLED(enablePreempting)
	{
		mmu = mem;
		processors = cpus;
		readyQueue = rq;
		numDispatchedJobs = 0;
		selectedProcessorIndex = 0;
	}

	~Dispatcher()
	{
		if (mmu != NULL)
			mmu = NULL;

		if (processors != NULL)
			processors = NULL;

		if (readyQueue != NULL)
			readyQueue = NULL;
	}

	bool canSelectProcess()
	{
		return selectedProcessQueue.size() < processors->size();
	}

	void selectProcessToDispatch(ProcessControlBlock selectedProcess)
	{
		//if (canSelectProcess())
		selectedProcessQueue.push(selectedProcess);
	}

	void dispatch(int processorIndex)
	{
		dispatchStartTime = std::chrono::high_resolution_clock::now();
		contextSwitch(&(processors->at(processorIndex)), selectedProcessQueue.pop());
		dispatchEndTime = std::chrono::high_resolution_clock::now();
		numDispatchedJobs++;
	}

	void dispatch()
	{
		// find idle processor
		int idleProcessorIndex = findIdleProcessor();
		bool idleProcessorFound = (idleProcessorIndex != -1);

		if (idleProcessorFound)
		{
			dispatch(idleProcessorIndex);
		}
		else
		{
			// if no idle processor exists and preempting enabled
			// then choose processor to preempt
			if (PREEMPTING_ENABLED)
			{
				dispatch(selectProcessor());
			}
			else
			{
				// processors are all busy; do nothing
			}
		}
	}

	int getNumDispatchedJobs()
	{
		return numDispatchedJobs;
	}

	bool isIdle()
	{
		return selectedProcessQueue.empty();
	}

	// dispatch latency:
	// the amount of time taken by the system to stop one process
	// and give permission to another process to begin execution
	std::chrono::duration<double> getDispatchLatency() const
	{
		//return timeToDuration(dispatchStartTime, dispatchEndTime);
		return std::chrono::duration_cast<std::chrono::duration<double>>(dispatchEndTime - dispatchStartTime);
	}

private:
	MemoryManagementUnit* mmu;
	std::vector<Processor> *processors;
	SafeQueue<ProcessControlBlock> selectedProcessQueue;
	ReadyQueue *readyQueue;
	int numDispatchedJobs;
	int selectedProcessorIndex;
	std::chrono::high_resolution_clock::time_point dispatchStartTime;
	std::chrono::high_resolution_clock::time_point dispatchEndTime;

	int findIdleProcessor()
	{
		int notFound = -1;

		for (size_t i = 0; i < processors->size(); i++)
		{
			if (processors->at(i).isIdle())
				return i;
		}

		return notFound;
	}

	int selectProcessor()
	{
		return (selectedProcessorIndex++ % processors->size());
	}

	void contextSwitch(Processor *processor, ProcessControlBlock newProcess)
	{
		// SAVE... ProcessContext
		if (!processor->isIdle())
		{
			ProcessControlBlock oldProcess = processor->stopRunningProcess();

			// TODO:: Dispatcher:: is ram full?
			// put process on ready queue and save to RAM
			if (readyQueue->size() < MAX_READY_QUEUE_SIZE)
			{
				switch (oldProcess.status)
				{
				case ProcessStatus::BLOCKED:
					// TODO:: Dispatcher ... is this necessary? oldProcess.schedulingInfo.setInputOutputEndTimeNow();
					//oldProcess.schedulingInfo.setInputOutputEndTimeNow();
					break;

				case ProcessStatus::RUNNING:
					oldProcess.schedulingInfo.setComputationEndTimeNow();
					break;

				default:
					break;
				}

				oldProcess.status = ProcessStatus::READY;
				oldProcess.context = processor->saveProcessContext();

				bool processInserted = mmu->insertProcess(MemoryType::RAM, oldProcess);

				if (processInserted)
				{
					readyQueue->push(oldProcess);
				}
				else
				{
					// TODO:: Dispatcher:: failed to insert process
					// try compact
					//mmu->compactRAM();
					// try insert again
				}
				
			}
			else
			{
				// TODO:: contextSwitch() ... ready queue full; do something
			}
		}

		// LOAD... ProcessContext
		newProcess.status = ProcessStatus::RUNNING;
		newProcess.schedulingInfo.setComputationStartTimeNow();

		processor->loadProcessContext(newProcess.context);
		processor->runProcess(newProcess);
	}
};
