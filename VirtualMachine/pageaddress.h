#pragma once

#include "word.h"

class PageAddress
{
public:
	int pageNumber; // p ... page number
	int offset;		// d ... offset (a.k.a displacement)

	PageAddress()
	{
		pageNumber = 0;
		offset = 0;
	}

	PageAddress(Word logicalAddress)
	{
		fromLogicalAddress(logicalAddress);
	}

	PageAddress(int pageNumber, int offset)
	{
		this->pageNumber = pageNumber;
		this->offset = offset;
	}

	~PageAddress()
	{}

	void fromLogicalAddress(Word logicalAddress)
	{
		// TODO:: PageAddress:: fromLogicalAddress()
		//pageNumber = logicalAddress / 
		//offset = logicalAddress %
	}

	std::string toString(std::string indent = "")
	{
		std::string retval = "";

		retval.append(indent);
		retval.append("Page Number (p):\t");
		retval.append(std::to_string(pageNumber));
		retval.append("\n");

		retval.append(indent);
		retval.append("Offset (d):\t");
		retval.append(std::to_string(offset));
		retval.append("\n");

		return retval;
	}

private:

};
