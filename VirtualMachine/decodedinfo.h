#pragma once

#include "ancillary.h"
#include "word.h"

class DecodedInfo
{
public:
	InstructionType instructionType;
	OpCode opCode;
	Word sourceRegister1;
	Word sourceRegister2;
	Word destinationRegister;
	Word baseRegister;
	Word register1;
	Word register2;
	Word address;

	DecodedInfo()
	{
		instructionType = static_cast<InstructionType>(-1);
		opCode = static_cast<OpCode>(-1);
	}

	~DecodedInfo()
	{}

	// Copy constructor
	DecodedInfo(const DecodedInfo &rhs)
	{
		instructionType = rhs.instructionType;
		opCode = rhs.opCode;
		sourceRegister1 = rhs.sourceRegister1;
		sourceRegister2 = rhs.sourceRegister2;
		destinationRegister = rhs.destinationRegister;
		baseRegister = rhs.baseRegister;
		register1 = rhs.register1;
		register2 = rhs.register2;
		address = rhs.address;
	}

	DecodedInfo& operator=(const DecodedInfo &rhs)
	{
		instructionType = rhs.instructionType;
		opCode = rhs.opCode;
		sourceRegister1 = rhs.sourceRegister1;
		sourceRegister2 = rhs.sourceRegister2;
		destinationRegister = rhs.destinationRegister;
		baseRegister = rhs.baseRegister;
		register1 = rhs.register1;
		register2 = rhs.register2;
		address = rhs.address;

		return *this;
	}

	std::string toAssemblyString(std::string indent = "")
	{
		std::string retval;

		retval.append(indent);
		retval.append(opCodeToString(opCode) + " ");

		switch (instructionType)
		{
		case InstructionType::ARITHMETIC_OR_REGISTER_TRANSFER:
			retval.append(sourceRegister1.toString() + " ");
			retval.append(sourceRegister2.toString() + " ");
			retval.append(destinationRegister.toString());
			break;

		case InstructionType::CONDITIONAL_BRANCH_AND_IMMEDIATE:
			retval.append(baseRegister.toString() + " ");
			retval.append(destinationRegister.toString() + " ");
			retval.append(address.toHexString(true, true));
			break;

		case InstructionType::UNCONDITIONAL_JUMP:
			retval.append(address.toHexString(true, true));
			break;

		case InstructionType::INPUT_AND_OUTPUT:
			retval.append(register1.toString() + " ");
			retval.append(register2.toString() + " ");
			retval.append(address.toHexString(true, true));
			break;

		default:
			// ERROR
			break;
		}

		retval.append("\n");

		return retval;
	}

	std::string toString(std::string indent = "")
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		retval.append(indent + "\t");
		retval.append("Instruction Type:\t");
		retval.append(instructionTypeToString(instructionType));
		retval.append("\n");

		retval.append(indent + "\t");
		retval.append("OpCode:\t\t\t");
		retval.append(opCodeToString(opCode));
		retval.append("\n");

		switch (instructionType)
		{
		case InstructionType::ARITHMETIC_OR_REGISTER_TRANSFER:
			retval.append(indent + "\t");
			retval.append("Source Register #1:\t");
			retval.append(sourceRegister1.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Source Register #2:\t");
			retval.append(sourceRegister2.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Destination Register:\t");
			retval.append(destinationRegister.toString());
			retval.append("\n");
			break;

		case InstructionType::CONDITIONAL_BRANCH_AND_IMMEDIATE:
			retval.append(indent + "\t");
			retval.append("Base Register:\t\t");
			retval.append(baseRegister.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Destination Register:\t");
			retval.append(destinationRegister.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Address:\t\t");
			retval.append(address.toHexString(true, true));
			retval.append("\n");
			break;

		case InstructionType::UNCONDITIONAL_JUMP:
			retval.append(indent + "\t");
			retval.append("Address:\t\t");
			retval.append(address.toHexString(true, true));
			retval.append("\n");
			break;

		case InstructionType::INPUT_AND_OUTPUT:
			retval.append(indent + "\t");
			retval.append("Register #1:\t\t");
			retval.append(register1.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Register #2:\t\t");
			retval.append(register2.toString());
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Address:\t\t");
			retval.append(address.toHexString(true, true));
			retval.append("\n");
			break;

		default:
			// ERROR
			break;
		}

		retval.append("\n");
		retval.append(indent);
		retval.append(".........................................");
		retval.append("\n");

		return retval;
	}

private:
};
