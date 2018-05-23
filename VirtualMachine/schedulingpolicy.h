#pragma once

enum class SchedulingPolicy
{
	FIRST_COME_FIRST_SERVED,	// non-preemptive, preemptive
	SHORTEST_JOB_FIRST,			// non-preemptive, preemptive
	PRIORITY,					// non-preemptive
	ROUND_ROBIN,				// preemptive
	MULTILEVEL_QUEUE,
	MULTILEVEL_FEEDBACK_QUEUE
};
