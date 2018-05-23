#pragma once

//#include "cache.h"
#include "registers.h"
#include "word.h"
#include <string>
#include <vector>

// TODO:: ** ProcessContext

// record of environment that is saved on interrupt, including...
// pc, registers, permissions, buffers, caches, active pages/blocks
class ProcessContext
{
public:
	// indicates the address of the next instruction to be executed for this process.
	Word programCounter;

	// CPU registers...
	// These vary in number and type, depending on the computer architecture.
	// They include accumulators, index registers, stack pointers, 
	// and general-purpose registers, plus any condition-code information.
	Registers registers;

	// TODO:: ProcessContext:: permissions

	// ------------------------------------------------------------------------------
	// Input Buffer		= contains data read by the program
	// Output Buffer	= contains data produced by the program
	// Temp Buffer		= area in memory to store/retrieve the data temporarily
	// ------------------------------------------------------------------------------
	std::vector<Word> inputBuffer;
	std::vector<Word> outputBuffer;
	std::vector<Word> temporaryBuffer;
	// TODO:: ProcessContext:: buffers ... use 'Memory' instead of 'vector<Word>' ???
	// TODO:: ProcessContext:: buffers ... if paging enabled

	// TODO:: ProcessContext:: caches ... if cache enabled
	//Cache *cpuCache = nullptr;

	// TODO:: ProcessContext:: active pages/blocks ... verify paging enabled

	ProcessContext()
	{
		programCounter = 0;
	}

	~ProcessContext()
	{
		//if (cpuCache != nullptr)
			//delete cpuCache;
	}

	// Copy constructor
	ProcessContext(const ProcessContext &rhs)
	{
		programCounter = rhs.programCounter;
		registers = rhs.registers;

		inputBuffer = rhs.inputBuffer;
		outputBuffer = rhs.outputBuffer;
		temporaryBuffer = rhs.temporaryBuffer;

		/*
		if (cpuCache == nullptr)
			cpuCache = new Memory(rhs.getCache());
		else
			*cpuCache = rhs.getCache();
		*/
	}

	ProcessContext& operator=(const ProcessContext &rhs)
	{
		programCounter = rhs.programCounter;
		registers = rhs.registers;

		inputBuffer = rhs.inputBuffer;
		outputBuffer = rhs.outputBuffer;
		temporaryBuffer = rhs.temporaryBuffer;

		/*
		if (cpuCache == nullptr)
			cpuCache = new Memory(rhs.getCache());
		else
			*cpuCache = rhs.getCache();
		*/

		return *this;
	}

	/*
	Memory getCache() const
	{
		Memory nullmemory(0);

		if (cpuCache != nullptr)
			return *cpuCache;
		else
			return nullmemory;
	}
	*/

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
		retval.append("Program Counter:\t");
		retval.append(programCounter.toHexString(true, true));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Registers:\t");
		retval.append(registers.toString(indent + "\t", numberSystem, usePrefix, useLeadingZeros));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Input Buffer:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&inputBuffer, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Output Buffer:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&outputBuffer, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Temporary Buffer:\n");
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");
		retval.append(Word::vectorToString(&temporaryBuffer, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
		retval.append(indent + "\t");
		retval.append("-----------------------------------------");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		return retval;
	}

private:
};
