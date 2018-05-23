#pragma once

#include "word.h"
#include <vector>

// http://www.selfgrowth.com/articles/cpu-registers-and-their-functions
// https://en.wikipedia.org/wiki/Accumulator_(computing)

#define NUMBER_OF_REGISTERS 16
#define ACCUMULATOR 0
#define ZERO_REGISTER 1

// starting from this register onward, all registers are general purpose
#define FIRST_GENERAL_PURPOSE_REGISTER 2

/*
	There are 16 registers; each 32 bit long

	Register [0] - Accumulator: holds the output of the Algorithmic Logic Unit (ALU)
	Register [1] - Zero Register: contains the value 0
	Registers [2] to [15] - General Purpose Register: can hold a 32 bit long value
*/
class Registers
{
public:
	Registers()
	{
		r = std::vector<Word>(NUMBER_OF_REGISTERS);

		r[ACCUMULATOR].setLabel("Accumulator");

		r[ZERO_REGISTER] = 0;
		r[ZERO_REGISTER].setLabel("Zero Register");

		for (int i = FIRST_GENERAL_PURPOSE_REGISTER; i < r.size(); i++)
		{
			r[i].setLabel("General Purpose");
			//r[i].setLabel("General Purpose Register");
		}
	}

	~Registers()
	{}

	// Copy constructor
	Registers(const Registers &rhs)
	{
		if (r.size() == rhs.size())
		{
			for (int i = 0; i < r.size(); i++)
			{
				write(i, rhs.read(i));
			}
		}
		else
		{
			// ERROR
			// registers are not the same size
		}
	}

	Registers& operator=(const Registers &rhs)
	{
		if (r.size() == rhs.size())
		{
			for (int i = 0; i < r.size(); i++)
			{
				write(i, rhs.read(i));
			}
		}
		else
		{
			// ERROR
			// registers are not the same size
		}

		return *this;
	}

	Word read(int registerNumber) const
	{
		return r[registerNumber];
	}

	void write(int registerNumber, Word newValue)
	{
		if (registerNumber < NUMBER_OF_REGISTERS)
		{
			switch (registerNumber)
			{
			case ACCUMULATOR:
				r[registerNumber] = newValue;
				break;

			case ZERO_REGISTER:
				// ERROR, zero register always equals zero
				break;

			default:
				r[registerNumber] = newValue;
				break;
			}
		}
		else
		{
			// ERROR, register number out of bounds
		}
	}

	int size() const
	{
		return r.size();
	}

	void changeByteOrder(ByteOrder byteOrder)
	{
		for (int i = 0; i < r.size(); i++)
		{
			r[i].endianess = byteOrder;
		}
	}

	std::string toString(
		std::string indent = "",
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		bool usePrefix = true,
		bool useLeadingZeros = true,
		bool useLabel = false)
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append("-----------------------------------------");
		retval.append("\n");

		switch (numberSystem)
		{
		case NumberSystem::BINARY:
			for (int i = 0; i < r.size(); i++)
			{
				retval.append(indent);
				retval.append("R" + std::to_string(i) + ":\t");
				retval.append(r[i].toBinaryString(usePrefix));

				if (useLabel)
				{
					retval.append("\tlabel( ");
					retval.append(r[i].getLabel());
					retval.append(" )");
				}
				
				retval.append("\n");
			}
			break;

		case NumberSystem::DECIMAL:
			for (int i = 0; i < r.size(); i++)
			{
				retval.append(indent);
				retval.append("R" + std::to_string(i) + ":\t");
				// IMPORTANT:: convert to signed int for negative numbers
				retval.append(std::to_string(r[i].toInt()));

				if (useLabel)
				{
					retval.append("\tlabel( ");
					retval.append(r[i].getLabel());
					retval.append(" )");
				}

				retval.append("\n");
			}
			break;

		case NumberSystem::HEXADECIMAL:
			for (int i = 0; i < r.size(); i++)
			{
				retval.append(indent);
				retval.append("R" + std::to_string(i) + ":\t");
				retval.append(r[i].toHexString(usePrefix, useLeadingZeros));

				if (useLabel)
				{
					retval.append("\tlabel( ");
					retval.append(r[i].getLabel());
					retval.append(" )");
				}

				retval.append("\n");
			}
			break;

		default:
			// ERROR
			break;
		}

		retval.append("\n");
		retval.append(indent);
		retval.append("-----------------------------------------");
		retval.append("\n");

		return retval;
	}

private:
	std::vector<Word> r;
};
