#pragma once

#include "dispatcher.h"
#include "loader.h"
#include "mmu.h"
#include "processor.h"
#include "scheduler.h"
#include "waitforinterrupt.h"
#include <queue>

class Driver
{
public:
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY;
	const MemoryAllocationPolicy MEMORY_ALLOCATION_POLICY;
	const bool DMA_MODE_ENABLED;
	const bool CPU_CACHE_ENABLED;
	const bool PREEMPTING_ENABLED;

	Driver(
		MemoryManagementPolicy mmp,
		MemoryAllocationPolicy map,
		SchedulingPolicy jobSchedPolicy,
		SchedulingPolicy cpuSchedPolicy,
		bool enableDMA = false,
		bool enablePreempting = false,
		int numProcessors = 1,
		unsigned long long cpuCacheSizeInBytes = 0,
		unsigned long long ramSizeInBytes = 4096,
		unsigned long long diskSizeInBytes = 8196) :
		MEMORY_MANAGEMENT_POLICY(mmp),
		MEMORY_ALLOCATION_POLICY(map),
		DMA_MODE_ENABLED(enableDMA),
		CPU_CACHE_ENABLED(cpuCacheSizeInBytes > 0),
		PREEMPTING_ENABLED(enablePreempting)
	{
		jobSchedulingPolicy = jobSchedPolicy;
		cpuSchedulingPolicy = cpuSchedPolicy;

		mmu = new MemoryManagementUnit(
			MEMORY_MANAGEMENT_POLICY, 
			MEMORY_ALLOCATION_POLICY, 
			ramSizeInBytes, 
			diskSizeInBytes
		);

		jobQueue = new SafeQueue<ProcessControlBlock>();
		readyQueue = new ReadyQueue(cpuSchedulingPolicy);
		waitQueue = new SafeQueue<ProcessControlBlock>();
		terminatedQueue = new SafeQueue<ProcessControlBlock>();

		loader = new Loader(mmu, jobQueue);
		loadingComplete = false;
		numLoadedJobs = 0;

		processors = new std::vector<Processor>();

		if (numProcessors <= 0)
		{
			std::cerr << "ERROR:: number of processors cannot be less than one" << std::endl;
			processors->push_back(Processor(mmu, waitQueue, terminatedQueue, DMA_MODE_ENABLED, cpuCacheSizeInBytes));
		}
		else
		{
			for (int i = 0; i < numProcessors; i++)
			{
				processors->push_back(Processor(mmu, waitQueue, terminatedQueue, DMA_MODE_ENABLED, cpuCacheSizeInBytes));
			}
		}

		dispatcher = new Dispatcher(mmu, processors, readyQueue, PREEMPTING_ENABLED);

		jobScheduler = new JobScheduler(mmu, jobSchedulingPolicy, jobQueue, readyQueue);
		cpuScheduler = new CpuScheduler(mmu, cpuSchedulingPolicy, readyQueue, dispatcher);
		
		start();
	}

	~Driver()
	{
		stop();

		delete processors;
		delete dispatcher;
		delete cpuScheduler;
		delete jobScheduler;
		delete loader;

		delete terminatedQueue;
		delete waitQueue;
		delete readyQueue;
		delete jobQueue;

		delete mmu;
	}
	
	void start(std::string programFile = "Program-File.txt")
	{
		systemStartTime = std::chrono::high_resolution_clock::now();
		running = true;

		// load program only once
		if (loadingComplete == false)
		{
			// load user programs, or jobs, which are already assembled
			// (given as a stream of hex character) and stored in a 'program-file.'
			loader->load(programFile);
			numLoadedJobs = loader->getNumLoadedJobs();
			loadingComplete = true;
		}
		
		// This cycle continues forever or until your simulator is shut down
		// when no more user programs are pending.
		while (running && (terminatedQueue->size() < numLoadedJobs))
		{
			// select a job from the job pool (on disk) to be loaded into main memory
			jobScheduler->schedule();

			// select a user program from a list of 'ready' jobs for dispatch.
			cpuScheduler->schedule();

			// places the selected job or program in an 'execution context'
			// to run on the virtual CPU.
			dispatcher->dispatch();

			// executes the programs in the simulated RAM.

			// if a program/job is interrupted while on the CPU, the interrupt
			// is serviced while the job is suspended. After the service is done,
			// the job must be returned to the 'ready-state', at some point in time.
			waitforinterrupt();

			// when a program completes, the Scheduler and the Dispatcher
			// process the next program.
		}
	}

	// TODO:: Driver:: forceStop(), requestStop()
	void stop()
	{
		// no more user programs are pending?
		systemStopTime = std::chrono::high_resolution_clock::now();
		running = false;
	}

	std::string getSystemUpTime() const
	{
		std::chrono::duration<double> uptime;
		
		if (running)
		{
			std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
			uptime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - systemStartTime);
		}
		else
		{
			uptime = std::chrono::duration_cast<std::chrono::duration<double>>(systemStopTime - systemStartTime);
		}

		return std::to_string(uptime.count());
	}

private:
	bool running;
	bool loadingComplete; // used to ensure loading occurs only once
	int numLoadedJobs;

	SchedulingPolicy jobSchedulingPolicy;
	SchedulingPolicy cpuSchedulingPolicy;

	std::chrono::high_resolution_clock::time_point systemStartTime;
	std::chrono::high_resolution_clock::time_point systemStopTime;
	
	// jobQueue			...list of all processes in the system
	// readyQueue		...list of all processes in main memory (ready & waiting to execute)
	// waitQueue		...list of processes waiting for an I/O device
	// terminatedQueue	...list of terminated processes
	SafeQueue<ProcessControlBlock> *jobQueue;
	ReadyQueue *readyQueue;
	SafeQueue<ProcessControlBlock> *waitQueue;
	SafeQueue<ProcessControlBlock> *terminatedQueue;

	MemoryManagementUnit *mmu;
	Loader *loader;
	JobScheduler *jobScheduler; // long-term scheduler
	CpuScheduler *cpuScheduler; // short-term scheduler
	Dispatcher *dispatcher;
	std::vector<Processor> *processors;
};