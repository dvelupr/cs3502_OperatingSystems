#pragma once

#include <string>
#include <vector>

#ifndef PAGE_SIZE_IN_WORDS
#define PAGE_SIZE_IN_WORDS 4
#endif

class PageTableEntry
{
public:
	int frameNumber;

	// used to determine if a page fault interrupt should be generated
	bool validBit;
	
	//bool modifyBit;
	//bool referenceBit;

	PageTableEntry()
	{
		frameNumber = 0;
		validBit = false;
		//modifyBit = false;
		//referenceBit = false;
	}

	~PageTableEntry()
	{}

	std::string toString(std::string indent = "", std::string delim = "\n")
	{
		std::string retval = "";

		retval.append(indent);
		retval.append("Frame #:\t\t");
		retval.append(std::to_string(frameNumber));
		retval.append(delim);

		retval.append(indent);
		retval.append("Valid Bit:\t\t");
		retval.append(std::to_string(validBit));
		retval.append(delim);

		/*
		retval.append(indent);
		retval.append("Modify Bit:\t\t");
		retval.append(std::to_string(modifyBit));
		retval.append(delim);
		*/

		/*
		retval.append(indent);
		retval.append("Reference Bit:\t\t");
		retval.append(std::to_string(referenceBit));
		retval.append(delim);
		*/

		return retval;
	}

	static std::string pageTableToString(
		const std::vector<PageTableEntry> *pageTable,
		std::string indent = "")
	{
		std::string retval;

		retval.append(indent);
		retval.append("-----------------------------------------");
		retval.append("\n");

		if (pageTable != NULL)
		{
			for (int i = 0; i < pageTable->size(); i++)
			{
				retval.append(indent);
				retval.append("Page #:\t\t");
				retval.append(std::to_string(i));
				retval.append("\n");

				PageTableEntry p = pageTable->at(i);
				retval.append(p.toString(indent));
				retval.append("\n");
			}
		}
		else
		{
			retval.append("NULL");
		}

		retval.append(indent);
		retval.append("-----------------------------------------");
		retval.append("\n");

		return retval;
	}

private:

};
