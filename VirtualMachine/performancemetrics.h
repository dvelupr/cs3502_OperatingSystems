#pragma once

#include "timeduration.h"
#include <string>

// TODO:: ** PerformanceMetrics

// ----------------------------------------------------------------
// Phase 1 Part 1
// ----------------------------------------------------------------

// Waiting and completion times for each job; and the averages of both metrics

// Number of I/O operations each process made

// Percentage of RAM space used and cache used

// Measure and compare the above performance metrics for 
// FIFO and Priority (non-preemptive) scheduling policies

// ----------------------------------------------------------------
// Phase 1 Part 2
// ----------------------------------------------------------------

// Waiting and completion times for jobs assigned to each CPU; and the averages of both

// Number of I/O operations each process made

// Which jobs and percentages of jobs assigned to each CPU; 
// RAM space used and cache used per CPU

// Measure, compare, and discuss the performance metrics for 
// 1-CPU and N-CPU runs, under the scheduling policies

// ----------------------------------------------------------------

class PerformanceMetrics
{
public:
	PerformanceMetrics()
	{}

	~PerformanceMetrics()
	{}

	std::string toString(std::string indent = "")
	{
		std::string retval = "";

		return retval;
	}

private:

};
