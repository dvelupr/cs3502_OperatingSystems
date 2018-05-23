#pragma once

#include "timeduration.h"
#include <chrono>
#include <string>

enum class TimeRecordingState
{
	NONE,
	COMPUTATION_BURST,
	IO_BURST,
	WAITING
};

/*
	Arrival Time:		the moment a process enters the ready queue

	Start Time:			the moment a process starts execution for the first time

	End Time:			the momemnt a process ends execution for the first time

	Completion Time:	the moment a process completes execution

	Turn Around Time:	the total time a process has spent in the system
	Turn_Around_Time = ( Completion_Time - Arrival_Time )

	Burst Time:			the amount of time required by a process to finish

	Waiting Time:		the amount of time a process had to wait before executing on the cpu
	Waiting_Time = ( Turn_Around_Time - Burst_Time )

	Response Time:		the first time a process had to wait before executing on the cpu
	Response_Time = ( Start_Time - Arrival_Time )
*/
class SchedulingInfo
{
public:
	short priority;

	// timeSlice

	// remainTime

	SchedulingInfo()
	{
		creationTime = std::chrono::high_resolution_clock::now();
		generateCreationTimeString();

		// START... waiting
		waitingStartTime = std::chrono::high_resolution_clock::now();
		timeRecordingState = TimeRecordingState::WAITING;

		priority = 0;
		loadTimeIsSet = arrivalTimeIsSet = firstStartTimeIsSet = firstEndTimeIsSet = completionTimeIsSet = false;
	}

	~SchedulingInfo()
	{
		/*
		// STOP... recording time if necessary
		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			break;

		case TimeRecordingState::CPU_BURST:
		case TimeRecordingState::IO_BURST:
			setCompletionTimeNow();
			break;

		default:
			// ERROR
			break;
		}
		*/
	}

	void generateCreationTimeString()
	{
		auto now = std::chrono::system_clock::now();
		auto creationTimeT = std::chrono::system_clock::to_time_t(now);

		// get the milliseconds part of creation time
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto fraction = now - seconds;
		auto creationTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();
		creationTimeString = std::ctime(&creationTimeT);

		// remove newline from end of string
		creationTimeString.pop_back();

		int n = creationTimeString.size();
		std::string year = creationTimeString.substr(n - 4);
		creationTimeString = creationTimeString.substr(0, n - 5) + " [" + std::to_string(creationTimeMilliseconds) + "ms] " + year;
	}

	void setLoadTimeNow()
	{
		if (loadTimeIsSet == false)
		{
			loadTime = std::chrono::high_resolution_clock::now();
			loadTimeIsSet = true;
		}
	}

	void setArrivalTimeNow()
	{
		if (arrivalTimeIsSet == false)
		{
			arrivalTime = std::chrono::high_resolution_clock::now();
			arrivalTimeIsSet = true;
		}
	}

	void setComputationStartTimeNow()
	{
		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			// ERROR
			// process complete
			break;

		case TimeRecordingState::COMPUTATION_BURST:
			// ERROR
			// computation burst already started
			break;

		case TimeRecordingState::IO_BURST:
			// ERROR
			// can't start computation burst until io burst is stopped
			break;

		case TimeRecordingState::WAITING:
			// STOP... waiting
			waitingEndTime = std::chrono::high_resolution_clock::now();

			if (firstStartTimeIsSet == false)
			{
				firstStartTime = waitingEndTime;
				firstStartTimeIsSet = true;
			}

			// START... computation burst
			computationStartTime = waitingEndTime;
			timeRecordingState = TimeRecordingState::COMPUTATION_BURST;

			// RECORD... waiting time
			waitingTimes.push_back(timeToDuration(waitingStartTime, waitingEndTime));
			break;

		default:
			// ERROR
			break;
		}
	}

	void setComputationEndTimeNow()
	{
		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			// ERROR
			// process complete
			break;

		case TimeRecordingState::COMPUTATION_BURST:
			if (firstEndTimeIsSet == false)
			{
				firstEndTime = std::chrono::high_resolution_clock::now();
				firstEndTimeIsSet = true;

				// STOP... computation burst
				computationEndTime = firstEndTime;
			}
			else
			{
				// STOP... computation burst
				computationEndTime = std::chrono::high_resolution_clock::now();
			}

			// START... waiting
			waitingStartTime = computationEndTime;
			timeRecordingState = TimeRecordingState::WAITING;

			// RECORD... computation burst time
			computationBurstTimes.push_back(timeToDuration(computationStartTime, computationEndTime));
			break;

		case TimeRecordingState::IO_BURST:
			// ERROR
			// can't stop computation burst because it wasn't started
			break;

		case TimeRecordingState::WAITING:
			// ERROR
			// computation burst already stopped
			break;

		default:
			// ERROR
			break;
		}
	}

	void setInputOutputStartTimeNow()
	{
		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			// ERROR
			// process complete
			break;

		case TimeRecordingState::COMPUTATION_BURST:
			// ERROR
			// can't start io burst until computation burst is stopped
			break;

		case TimeRecordingState::IO_BURST:
			// ERROR
			// io burst already started
			break;

		case TimeRecordingState::WAITING:
			// STOP... waiting
			waitingEndTime = std::chrono::high_resolution_clock::now();

			// START... io burst
			ioStartTime = waitingEndTime;
			timeRecordingState = TimeRecordingState::IO_BURST;

			// RECORD... waiting time
			waitingTimes.push_back(timeToDuration(waitingStartTime, waitingEndTime));
			break;

		default:
			// ERROR
			break;
		}
	}

	void setInputOutputEndTimeNow()
	{
		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			// ERROR
			// process complete
			break;

		case TimeRecordingState::COMPUTATION_BURST:
			// ERROR
			// can't stop io burst because it wasn't started
			break;

		case TimeRecordingState::IO_BURST:
			// STOP... io burst
			ioEndTime = std::chrono::high_resolution_clock::now();

			// START... waiting
			waitingStartTime = ioEndTime;
			timeRecordingState = TimeRecordingState::WAITING;

			// RECORD... io burst time
			ioBurstTimes.push_back(timeToDuration(ioStartTime, ioEndTime));
			break;

		case TimeRecordingState::WAITING:
			// ERROR
			// io burst already stopped
			break;

		default:
			// ERROR
			break;
		}
	}

	void setCompletionTimeNow()
	{
		if (completionTimeIsSet == false)
		{
			completionTime = std::chrono::high_resolution_clock::now();
			completionTimeIsSet = true;
		}

		switch (timeRecordingState)
		{
		case TimeRecordingState::NONE:
			// time recording stopped; do nothing
			break;

		case TimeRecordingState::COMPUTATION_BURST:
			// STOP... computation burst
			computationEndTime = std::chrono::high_resolution_clock::now();

			// RECORD... computation burst time
			computationBurstTimes.push_back(timeToDuration(computationStartTime, computationEndTime));
			break;

		case TimeRecordingState::IO_BURST:
			// STOP... io burst
			ioEndTime = std::chrono::high_resolution_clock::now();

			// RECORD... io burst time
			ioBurstTimes.push_back(timeToDuration(ioStartTime, ioEndTime));
			break;

		case TimeRecordingState::WAITING:
			// STOP... waiting
			waitingEndTime = std::chrono::high_resolution_clock::now();

			// RECORD... waiting time
			waitingTimes.push_back(timeToDuration(waitingStartTime, waitingEndTime));
			break;

		default:
			// ERROR
			break;
		}

		// STOP RECORDING TIME
		timeRecordingState = TimeRecordingState::NONE;
	}

	std::chrono::duration<double> getLoadTime() const
	{
		return timeToDuration(creationTime, loadTime);
	}

	std::chrono::duration<double> getArrivalTime() const
	{
		return timeToDuration(creationTime, arrivalTime);
	}

	std::chrono::duration<double> getStartTime() const
	{
		return timeToDuration(creationTime, firstStartTime);
	}

	std::chrono::duration<double> getEndTime() const
	{
		return timeToDuration(creationTime, firstEndTime);
	}

	std::chrono::duration<double> getCompletionTime() const
	{
		return timeToDuration(creationTime, completionTime);
	}

	std::chrono::duration<double> getTurnAroundTime() const
	{
		//return timeToDuration(arrivalTime, completionTime);
		return std::chrono::duration_cast<std::chrono::duration<double>>(completionTime - arrivalTime);
	}

	std::chrono::duration<double> getFirstBurstTime() const
	{
		//return timeToDuration(startTime, endTime);
		return std::chrono::duration_cast<std::chrono::duration<double>>(firstEndTime - firstStartTime);
	}

	std::chrono::duration<double> getFirstWaitingTime() const
	{
		return (getTurnAroundTime() - getFirstBurstTime());
	}

	std::vector<std::chrono::duration<double>> getComputationBurstTimes()
	{
		return computationBurstTimes;
	}

	std::vector<std::chrono::duration<double>> getInputOutputBurstTimes()
	{
		return ioBurstTimes;
	}

	std::vector<std::chrono::duration<double>> getWaitingTimes()
	{
		return waitingTimes;
	}

	std::chrono::duration<double> getResponseTime() const
	{
		//return timeToDuration(arrivalTime, startTime);
		return std::chrono::duration_cast<std::chrono::duration<double>>(firstStartTime - arrivalTime);
	}

	std::string toString(std::string indent = "", TimeUnits timeUnits = TimeUnits::NANOSECONDS)
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		retval.append(indent);
		retval.append("Priority:\t\t");
		retval.append(std::to_string(priority));
		retval.append("\n");

		retval.append(indent);
		retval.append("Creation Time:\t\t");
		retval.append(creationTimeString);
		retval.append("\n");

		retval.append(indent);
		retval.append("Load Time:\t\t");
		retval.append(timeDurationToString(creationTime, loadTime, timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("Arrival Time:\t\t");
		retval.append(timeDurationToString(creationTime, arrivalTime, timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("First Start Time:\t");
		retval.append(timeDurationToString(creationTime, firstStartTime, timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("First End Time:\t\t");
		retval.append(timeDurationToString(creationTime, firstEndTime, timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("Completion Time:\t");
		retval.append(timeDurationToString(creationTime, completionTime, timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("Turn Around Time:\t");
		retval.append(timeDurationToString(getTurnAroundTime(), timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("First Burst Time:\t");
		retval.append(timeDurationToString(getFirstBurstTime(), timeUnits));
		retval.append("\n");

		retval.append(indent);
		retval.append("First Waiting Time:\t");
		retval.append(timeDurationToString(getFirstWaitingTime(), timeUnits));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Computation Burst Times:");
		retval.append("\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(vectorTimeDurationToString(computationBurstTimes, timeUnits, indent + "\t"));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("I/O Burst Times:");
		retval.append("\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(vectorTimeDurationToString(ioBurstTimes, timeUnits, indent + "\t"));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Waiting Times:");
		retval.append("\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(vectorTimeDurationToString(waitingTimes, timeUnits, indent + "\t"));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Response Time:\t\t");
		retval.append(timeDurationToString(getResponseTime(), timeUnits));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		return retval;
	}

private:
	std::string creationTimeString;
	std::chrono::high_resolution_clock::time_point creationTime;
	std::chrono::high_resolution_clock::time_point loadTime;
	std::chrono::high_resolution_clock::time_point arrivalTime;
	std::chrono::high_resolution_clock::time_point firstStartTime;
	std::chrono::high_resolution_clock::time_point firstEndTime;
	std::chrono::high_resolution_clock::time_point completionTime;

	// bools are used to ensure respective time_point is only set once
	bool loadTimeIsSet;
	bool arrivalTimeIsSet;
	bool firstStartTimeIsSet;
	bool firstEndTimeIsSet;
	bool completionTimeIsSet;

	TimeRecordingState timeRecordingState;
	std::chrono::high_resolution_clock::time_point computationStartTime;
	std::chrono::high_resolution_clock::time_point computationEndTime;
	std::chrono::high_resolution_clock::time_point ioStartTime;
	std::chrono::high_resolution_clock::time_point ioEndTime;
	std::chrono::high_resolution_clock::time_point waitingStartTime;
	std::chrono::high_resolution_clock::time_point waitingEndTime;
	std::vector<std::chrono::duration<double>> computationBurstTimes;
	std::vector<std::chrono::duration<double>> ioBurstTimes;
	std::vector<std::chrono::duration<double>> waitingTimes;

	/*
	// Copy constructor
	SchedulingInfo(const SchedulingInfo &rhs)
	{

	}

	SchedulingInfo& operator=(const SchedulingInfo &rhs)
	{

		return *this;
	}
	*/
};
