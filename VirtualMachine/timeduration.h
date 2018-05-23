#pragma once

#include <chrono>
#include <string>

static std::chrono::duration<double> timeToDuration(
	std::chrono::high_resolution_clock::time_point startTime,
	std::chrono::high_resolution_clock::time_point endTime)
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
}

enum class TimeUnits
{
	NANOSECONDS,
	MICROSECONDS,
	MILLISECONDS,
	SECONDS,
	MINUTES,
	HOURS
};

static std::string timeDurationToString(
	std::chrono::duration<double> timeDuration,
	TimeUnits timeUnits = TimeUnits::NANOSECONDS)
{
	if (timeDuration.count() < 0)
		return "NULL";

	std::string retval;

	switch (timeUnits)
	{
	case TimeUnits::NANOSECONDS:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(timeDuration).count());
		retval.append(" nanoseconds");
		break;

	case TimeUnits::MICROSECONDS:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(timeDuration).count());
		retval.append(" microseconds");
		break;

	case TimeUnits::MILLISECONDS:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeDuration).count());
		retval.append(" milliseconds");
		break;

	case TimeUnits::SECONDS:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(timeDuration).count());
		retval.append(" seconds");
		break;

	case TimeUnits::MINUTES:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(timeDuration).count());
		retval.append(" minutes");
		break;

	case TimeUnits::HOURS:
		retval = std::to_string(std::chrono::duration_cast<std::chrono::hours>(timeDuration).count());
		retval.append(" hours");
		break;

	default:
		retval = "ERROR:: Unkown Time Units " + static_cast<int>(timeUnits);
		break;
	}

	return retval;
}

static std::string timeDurationToString(
	std::chrono::high_resolution_clock::time_point startTime,
	std::chrono::high_resolution_clock::time_point endTime,
	TimeUnits timeUnits = TimeUnits::NANOSECONDS)
{
	std::string retval = "";
	std::chrono::duration<double> td = timeToDuration(startTime, endTime);

	if (td.count() < 0)
		retval = "NULL";
	else
		retval = timeDurationToString(td, timeUnits);

	return retval;
}

static std::string vectorTimeDurationToString(
	std::vector<std::chrono::duration<double>> timeDurationVector,
	TimeUnits timeUnits = TimeUnits::NANOSECONDS,
	std::string indent = "",
	std::string delim = "\n")
{
	std::string retval = "";

	if (timeDurationVector.empty())
	{
		retval.append(indent);
		retval.append("EMPTY");
		retval.append(delim);
	}
	else
	{
		for (size_t i = 0; i < timeDurationVector.size(); i++)
		{
			retval.append(indent);
			retval.append(timeDurationToString(timeDurationVector[i], timeUnits));
			retval.append(delim);
		}
	}

	return retval;
}