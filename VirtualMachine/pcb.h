#pragma once

#include "memoryinfo.h"
#include "processcontext.h"
#include "registers.h"
#include "safequeue.h"
#include "schedulinginfo.h"
#include <queue>
#include <string>
#include <vector>

//#define DEFAULT_STACK_SIZE_IN_WORDS 40
//#define DEFAULT_HEAP_SIZE_IN_WORDS 40

// TODO:: ** PCB

/*
typedef struct PCB
{
	cpuid:				// information the assigned CPU (for multiprocessor system)
	program-counter		// the job’s pc holds the address of the instruction to fetch
	struct state:		// record of environment that is saved on interrupt
						// including the pc, registers, permissions, buffers, caches, active
						// pages/blocks
	code-size;			// extracted from the //JOB control line
	struct registers:	// accumulators, index, general
	struct sched:		// burst-time, priority, queue-type, time-slice, remain-time
	struct accounts:	// cpu-time, time-limit, time-delays, start/end times, io-times
	struct memories:	// page-table-base, pages, page-size
						// base-registers – logical/physical map, limit-reg
	struct progeny:		// child-procid, child-code-pointers
	parent: ptr;		// pointer to parent (if this process is spawned, else ‘null’)
	struct resources:	// file-pointers, io-devices – unitclass, unit#, open-file-tables
	status;				// {running, ready, blocked, new}
	status_info:		// pointer to ‘ready-list of active processes’ or
						// ‘resource-list on blocked processes’
	priority: integer;	// of the process, extracted from the //JOB control line
}
*/
class ProcessControlBlock
{
public:
	// serves as the repository for any info that may vary from process to process

	// pid
	short processId;

	// Note: only one process can be running on any processor at any instant
	ProcessStatus status;

	// TODO:: PCB:: status info
	// status_info;

	// the assigned CPU (for multiprocessor system)
	//short assignedCpuId;

	// Accounting information...
	// This includes the amount of CPU and real time used, time limits,
	// account numbers, job or process numbers, and so on.
	int numInputOutputOperations;

	// TODO:: percentage of RAM & cache used
	// throughput
	// cpu utilization

	// CPU scheduling information...
	// This includes a process priority, pointers to scheduling queues,
	// and any other scheduling parameters.
	SchedulingInfo schedulingInfo;

	// Memory management information...
	// address space of this process
	// This may include the value of the base and limit registers and the page tables,
	// or the segment tables, depending on the memory system used by the OS.
	MemoryInfo memoryInfo;

	// TODO:: PCB:: ResourceInfo
	// I/O status information...
	// This includes the list of I/O devices allocated to the process, 
	// a list of open files, and so on.
	//ResourceInfo resourceInfo;

	ProcessContext context;

	// this process's parent
	// ProcessControlBlock *parent = nullptr;

	// TODO:: PCB:: vector<ProcessControlBlock> children;
	// this process's children
	// progeny;

	ProcessControlBlock()
	{
		processId = 0;
		status = ProcessStatus::NEW;
		//assignedCpuId = 0;
		numInputOutputOperations = 0;
	}

	~ProcessControlBlock()
	{
		// TODO:: PCB:: terminate children?
	}

	// Copy constructor
	ProcessControlBlock(const ProcessControlBlock &rhs)
	{
		processId = rhs.processId;
		status = rhs.status;
		//assignedCpuId = rhs.assignedCpuId;
		numInputOutputOperations = rhs.numInputOutputOperations;
		schedulingInfo = rhs.schedulingInfo;
		memoryInfo = rhs.memoryInfo;
		//resourceInfo = rhs.resourceInfo;
		context = rhs.context;
		//parent = rhs.parent;
	}

	ProcessControlBlock& operator=(const ProcessControlBlock &rhs)
	{
		processId = rhs.processId;
		status = rhs.status;
		//assignedCpuId = rhs.assignedCpuId;
		numInputOutputOperations = rhs.numInputOutputOperations;
		schedulingInfo = rhs.schedulingInfo;
		memoryInfo = rhs.memoryInfo;
		//resourceInfo = rhs.resourceInfo;
		context = rhs.context;
		//parent = rhs.parent;

		return *this;
	}

	std::string toString(std::string indent = "")
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append("=========================================");
		retval.append("\n");

		retval.append(indent);
		retval.append("Process ID:\t\t");
		retval.append(std::to_string(processId));
		retval.append("\n");

		retval.append(indent);
		retval.append("Status:\t\t\t");
		retval.append(processStatusToString(status));
		retval.append("\n");

		/*
		retval.append(indent);
		retval.append("Assigned CPU ID:\t");
		retval.append(std::to_string(assignedCpuId));
		retval.append("\n");
		*/

		retval.append(indent);
		retval.append("Num I\/O Operations:\t");
		retval.append(std::to_string(numInputOutputOperations));
		retval.append("\n");

		retval.append(indent);
		retval.append("Scheduling Info:\t");
		retval.append(schedulingInfo.toString(indent + "\t", TimeUnits::MICROSECONDS));
		retval.append("\n");

		retval.append(indent);
		retval.append("Memory Info:\t");
		retval.append(memoryInfo.toString(indent + "\t"));
		retval.append("\n");

		/*
		retval.append(indent);
		retval.append("Resource Info:\t");
		retval.append(resourceInfo.toString(indent + "\t"));
		retval.append("\n");
		*/

		retval.append(indent);
		retval.append("Context:\t");
		retval.append(context.toString(indent + "\t"));
		retval.append("\n");

		/*
		retval.append(indent);
		retval.append("Parent:\t");
		retval.append(pointerToString(parent, indent));
		retval.append("\n");
		*/

		retval.append(indent);
		retval.append("=========================================");
		retval.append("\n");

		return retval;
	}

	static std::string queueToString(std::queue<ProcessControlBlock> pcbQueue, std::string indent = "")
	{
		std::string retval;
		std::queue<ProcessControlBlock> tempQueue = pcbQueue;

		for (int i = 0; i < tempQueue.size(); i++)
		{
			retval.append(tempQueue.front().toString());
			tempQueue.pop();
		}

		return retval;
	}

	static std::string queueToString(SafeQueue<ProcessControlBlock> pcbQueue, std::string indent = "")
	{
		std::string retval;
		SafeQueue<ProcessControlBlock> tempQueue = pcbQueue;

		for (int i = 0; i < tempQueue.size(); i++)
			retval.append(tempQueue.pop().toString());

		return retval;
	}

private:
};
