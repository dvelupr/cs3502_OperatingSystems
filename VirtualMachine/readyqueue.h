#pragma once

#include "schedulingpolicy.h"
#include <mutex>
#include <thread>

#ifndef MAX_READY_QUEUE_SIZE
#define MAX_READY_QUEUE_SIZE 1000000
#endif

class CompareReadyQueue
{
public:
	CompareReadyQueue(
		SchedulingPolicy schedPolicy = SchedulingPolicy::FIRST_COME_FIRST_SERVED,
		const bool& revparam = false)
	{
		schedulingPolicy = schedPolicy;
		reverse = revparam;
	}

	bool operator() (const ProcessControlBlock& lhs, const ProcessControlBlock&rhs) const
	{
		bool retval = false;

		if (reverse)
		{
			switch (schedulingPolicy)
			{
			case SchedulingPolicy::FIRST_COME_FIRST_SERVED:
				retval = lhs.schedulingInfo.getArrivalTime() < rhs.schedulingInfo.getArrivalTime();
				break;

			case SchedulingPolicy::SHORTEST_JOB_FIRST:
				// TODO:: SchedulingPolicy::SHORTEST_JOB_FIRST ... does jobsize == codesize ?
				if (lhs.memoryInfo.codeSizeInWords != rhs.memoryInfo.codeSizeInWords)
				{
					retval = lhs.memoryInfo.codeSizeInWords < rhs.memoryInfo.codeSizeInWords;
				}
				else
				{
					retval = lhs.schedulingInfo.priority < rhs.schedulingInfo.priority;
				}
				break;

			case SchedulingPolicy::PRIORITY:
				if (lhs.schedulingInfo.priority != rhs.schedulingInfo.priority)
				{
					retval = lhs.schedulingInfo.priority < rhs.schedulingInfo.priority;
				}
				else
				{
					retval = lhs.memoryInfo.codeSizeInWords < rhs.memoryInfo.codeSizeInWords;
				}
				break;

			case SchedulingPolicy::ROUND_ROBIN:
			case SchedulingPolicy::MULTILEVEL_QUEUE:
			case SchedulingPolicy::MULTILEVEL_FEEDBACK_QUEUE:
			default:
				// ERROR
				break;
			}
		}
		else
		{
			switch (schedulingPolicy)
			{
			case SchedulingPolicy::FIRST_COME_FIRST_SERVED:
				retval = lhs.schedulingInfo.getArrivalTime() > rhs.schedulingInfo.getArrivalTime();
				break;

			case SchedulingPolicy::SHORTEST_JOB_FIRST:
				if (lhs.memoryInfo.codeSizeInWords != rhs.memoryInfo.codeSizeInWords)
				{
					retval = lhs.memoryInfo.codeSizeInWords > rhs.memoryInfo.codeSizeInWords;
				}
				else
				{
					retval = lhs.schedulingInfo.priority > rhs.schedulingInfo.priority;
				}
				break;

			case SchedulingPolicy::PRIORITY:
				if (lhs.schedulingInfo.priority != rhs.schedulingInfo.priority)
				{
					retval = lhs.schedulingInfo.priority > rhs.schedulingInfo.priority;
				}
				else
				{
					retval = lhs.memoryInfo.codeSizeInWords > rhs.memoryInfo.codeSizeInWords;
				}
				break;

			case SchedulingPolicy::ROUND_ROBIN:
			case SchedulingPolicy::MULTILEVEL_QUEUE:
			case SchedulingPolicy::MULTILEVEL_FEEDBACK_QUEUE:
			default:
				// ERROR
				break;
			}
		}

		return retval;
	}

private:
	SchedulingPolicy schedulingPolicy;
	bool reverse;
};

typedef std::priority_queue<ProcessControlBlock, std::vector<ProcessControlBlock>, CompareReadyQueue> ReadyPriorityQueue;

class ReadyQueue
{
public:
	ReadyQueue(SchedulingPolicy schedPolicy)
	{
		schedulingPolicy = schedPolicy;

		if(schedulingPolicy == SchedulingPolicy::FIRST_COME_FIRST_SERVED)
			readyPriorityQueue = ReadyPriorityQueue(CompareReadyQueue(schedulingPolicy, true));
		else
			readyPriorityQueue = ReadyPriorityQueue(CompareReadyQueue(schedulingPolicy));
	}

	~ReadyQueue()
	{}

	bool empty() const
	{
		return readyPriorityQueue.empty();
	}

	ProcessControlBlock pop()
	{
		ProcessControlBlock retval;

		std::unique_lock<std::mutex> locker(mut);
		mut_condition.wait(locker, [&]() { return !readyPriorityQueue.empty(); });
		retval = readyPriorityQueue.top();
		readyPriorityQueue.pop();
		locker.unlock();

		return retval;
	}

	void push(const ProcessControlBlock& value)
	{
		std::unique_lock<std::mutex> locker(mut);
		readyPriorityQueue.push(value);
		locker.unlock();

		mut_condition.notify_all();
	}

	int size() const
	{
		return readyPriorityQueue.size();
	}

	std::string toString()
	{
		std::string retval;
		ReadyPriorityQueue tempQueue = readyPriorityQueue;

		for (int i = 0; i < tempQueue.size(); i++)
		{
			ProcessControlBlock p = tempQueue.top();
			retval.append(p.toString());
			tempQueue.pop();
		}

		return retval;
	}

private:
	ReadyPriorityQueue readyPriorityQueue;
	SchedulingPolicy schedulingPolicy;
	std::mutex mut;
	std::condition_variable mut_condition;
};
