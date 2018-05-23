#pragma once

// TODO:: SchedulingSystem

// SMP == SYMMETRIC_MULTI_PROCESSOR ... each processor is self scheduling

// ASYMMETRIC				...only one processor accesses the system data structures
// SMP_GLOBAL_READY_QUEUE	...all processes in common ready queue
// SMP_LOCAL_READY_QUEUE	...each processor has its own private ready queue
// SMP_HYBRID_READY_QUEUE	...distribute processes from global ready queue to local ready queues
enum class SchedulingSystem
{
	ASYMMETRIC,
	SMP_GLOBAL_READY_QUEUE,
	SMP_LOCAL_READY_QUEUE,
	SMP_HYBRID_READY_QUEUE
};
