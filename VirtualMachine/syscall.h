#pragma once

#include "mmu.h"
#include "pcb.h"
#include "safequeue.h"

// TODO:: syscall:: storage location of system call parameters
enum class SyscallParametersLocation
{
	REGISTERS,
	MEMORY_BLOCK,
	STACK
};

namespace syscall
{
	//============================
	// Process Control
	//============================
	
	// TODO:: fork()
	// TODO:: exec()

	// TODO:: createProcess()
	static ProcessControlBlock createProcess()
	{
		
	}

	// TODO:: wait()
	static void wait(
		OpCode opCode, 
		ProcessControlBlock *process,
		SafeQueue<ProcessControlBlock> *waitQueue)
	{
		if (process != NULL)
		{
			// TODO:: wait() ... save process context
			process->status = ProcessStatus::BLOCKED;
			process->schedulingInfo.setComputationEndTimeNow();
			waitQueue->push(*process);

			// IMPORTANT: stop pointing to process
			process = NULL;
		}
	}

	// TODO:: end()
	// halt program execution normally
	static void end(
		ProcessControlBlock *process,
		MemoryManagementUnit *mmu,
		SafeQueue<ProcessControlBlock> *terminatedQueue)
	{
		if (process != NULL)
		{
			process->schedulingInfo.setCompletionTimeNow();
			process->status = ProcessStatus::TERMINATED;

			mmu->removeProcess(MemoryType::RAM, *process);
			terminatedQueue->push(*process);

			// IMPORTANT: delete process last!
			delete process;
		}
	}

	// halt program execution abnormally
	static void abort(
		ProcessControlBlock *process, 
		MemoryManagementUnit *mmu,
		SafeQueue<ProcessControlBlock> *terminatedQueue)
	{
		// TODO:: abort()
		end(process, mmu, terminatedQueue);
	}

	// TODO:: exit()

	//============================
	// File Manipulation
	//============================

	//============================
	// Device Manipulation
	//============================

	//============================
	// Information Maintenance
	//============================

	//============================
	// Communications
	//============================

	//============================
	// Protection
	//============================

}
