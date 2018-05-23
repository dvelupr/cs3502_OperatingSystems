#pragma once

#include <string>

#ifndef HOLE_LABEL
#define HOLE_LABEL "Hole"
#endif

class Hole
{
public:
	// inclusive
	int start;
	int end;

	Hole(int start, int end)
	{
		this->start = start;
		this->end = end;
	}

	~Hole()
	{}

	int size() const
	{
		return end - start;
	}

	std::string toString()
	{
		return ("(" + std::to_string(start) + ", " + std::to_string(end) + ")");
	}

private:

};
