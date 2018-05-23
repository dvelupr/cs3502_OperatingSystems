#pragma once

#include "pagetableentry.h"
#include "word.h"
#include <boost/timer/timer.hpp>
#include <string>
#include <vector>

// TODO:: ** MemoryInfo; add buffers?, add cache?
class MemoryInfo
{
public:
	// page-table-base, pages, page-size

	std::vector<PageTableEntry> *pageTable = nullptr;

	//Word pageTableBaseRegister;
	int numPages;
	//int pageSizeInWords;

	// TODO:: MemoryInfo:: should the base and limit registers be of type 'Address'?
	// a pair of base and limit registers define the logical address space
	Word baseRegister;
	Word limitRegister;

	int codeSizeInWords;
	int dataSizeInWords;

	// buffers
	int inputBufferSize;
	int outputBufferSize;
	int temporaryBufferSize;

	// ------------------------------------------------------------------------------
	// Start Address for Buffers:
	// ------------------------------------------------------------------------------
	// Input Buffer		= codeSize * WORD_SIZE_IN_BYTES
	// Output Buffer	= inputBufferStartAddress + inputBufferSize * WORD_SIZE_IN_BYTES
	// Temp Buffer		= outputBufferStartAddress + outputBufferSize * WORD_SIZE_IN_BYTES
	// ------------------------------------------------------------------------------
	Word inputBufferStartAddress;
	Word outputBufferStartAddress;

	// TODO:: fix temporaryBufferStartAddress?
	Word temporaryBufferStartAddress;

	// TODO:: MemoryInfo:: should I remove these sections? should they only reside in (simulated) memory?

	// text		...program code
	// data		...contains global variables
	// stack	...contains temporary data (e.g. function parameters, return addresses, & local variables)
	// heap		...memory dynamically allocated during run time
	std::vector<Word> text;
	std::vector<Word> data;

	// TODO:: MemoryInfo:: do we ever use stack/heap in this simulation?
	std::vector<Word> stack;
	std::vector<Word> heap;

	MemoryInfo()
	{
		numPages = 4;
		pageTable = new std::vector<PageTableEntry>(numPages);

		codeSizeInWords = 0;
		dataSizeInWords = 0;
	}

	~MemoryInfo()
	{
		if (pageTable != nullptr)
			delete pageTable;
	}

	// Copy constructor
	MemoryInfo(const MemoryInfo &rhs)
	{
		baseRegister = rhs.baseRegister;
		limitRegister = rhs.limitRegister;

		codeSizeInWords = rhs.codeSizeInWords;
		dataSizeInWords = rhs.dataSizeInWords;

		inputBufferSize = rhs.inputBufferSize;
		outputBufferSize = rhs.outputBufferSize;
		temporaryBufferSize = rhs.temporaryBufferSize;

		inputBufferStartAddress = rhs.inputBufferStartAddress;
		outputBufferStartAddress = rhs.outputBufferStartAddress;
		temporaryBufferStartAddress = rhs.temporaryBufferStartAddress;

		text = rhs.text;
		data = rhs.data;
		stack = rhs.stack;
		heap = rhs.heap;
	}

	MemoryInfo& operator=(const MemoryInfo &rhs)
	{
		baseRegister = rhs.baseRegister;
		limitRegister = rhs.limitRegister;

		codeSizeInWords = rhs.codeSizeInWords;
		dataSizeInWords = rhs.dataSizeInWords;

		inputBufferSize = rhs.inputBufferSize;
		outputBufferSize = rhs.outputBufferSize;
		temporaryBufferSize = rhs.temporaryBufferSize;

		inputBufferStartAddress = rhs.inputBufferStartAddress;
		outputBufferStartAddress = rhs.outputBufferStartAddress;
		temporaryBufferStartAddress = rhs.temporaryBufferStartAddress;

		text = rhs.text;
		data = rhs.data;
		stack = rhs.stack;
		heap = rhs.heap;

		return *this;
	}

	std::string toString(
		std::string indent = "",
		int wordsPerLine = 16,
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		ByteOrder endianess = ByteOrder::BIG_ENDIAN,
		bool usePrefix = true,
		bool useLeadingZeros = true)
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		retval.append(indent);
		retval.append("Base Register:\t\t\t");
		retval.append(baseRegister.toHexString(usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append(indent);
		retval.append("Limit Register:\t\t\t");
		retval.append(limitRegister.toHexString(usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append(indent);
		retval.append("Code Size (in Words):\t\t");
		retval.append(std::to_string(codeSizeInWords));
		retval.append("\n");

		retval.append(indent);
		retval.append("Data Size (in Words):\t\t");
		retval.append(std::to_string(dataSizeInWords));
		retval.append("\n");

		retval.append(indent);
		retval.append("Input Buffer Size:\t\t");
		retval.append(std::to_string(inputBufferSize));
		retval.append("\n");

		retval.append(indent);
		retval.append("Output Buffer Size:\t\t");
		retval.append(std::to_string(outputBufferSize));
		retval.append("\n");

		retval.append(indent);
		retval.append("Temporary Buffer Size:\t\t");
		retval.append(std::to_string(temporaryBufferSize));
		retval.append("\n");

		retval.append(indent);
		retval.append("Input Buffer Start Address:\t");
		retval.append(inputBufferStartAddress.toHexString(usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append(indent);
		retval.append("Output Buffer Start Address:\t");
		retval.append(outputBufferStartAddress.toHexString(usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append(indent);
		retval.append("Temporary Buffer Start Address:\t");
		retval.append(temporaryBufferStartAddress.toHexString(usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Text:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&text, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		
		retval.append("\n");
		retval.append(indent);
		retval.append("Data:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&data, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Stack:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&stack, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Heap:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&heap, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Number of Pages:\t\t");
		retval.append(std::to_string(numPages));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Page Table:\n");
		retval.append(PageTableEntry::pageTableToString(pageTable, indent + "\t"));
		retval.append("\n");

		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		return retval;
	}

private:
};
