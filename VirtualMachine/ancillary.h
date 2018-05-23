#pragma once

#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define NOT_A_NUMBER "NAN"

#define BINARY_PREFIX "0b"
//#define OCTAL_PREFIX "0"
//#define DECIMAL_PREFIX ""
#define HEXADECIMAL_PREFIX "0x"

// each word is 4 bytes (i.e. 32 bits or 8 hex characters) long
#ifndef WORD_SIZE_IN_BITS
#define WORD_SIZE_IN_BITS 32
#endif // WORD_SIZE_IN_BITS

#ifndef convertBitsToBytes
#define convertBitsToBytes(bits) (bits / 8)
#endif // convertBitsToBytes

#ifndef WORD_SIZE_IN_BYTES
#define WORD_SIZE_IN_BYTES convertBitsToBytes(WORD_SIZE_IN_BITS)
#endif // WORD_SIZE_IN_BYTES

#ifndef convertBytesToWords
#define convertBytesToWords(bytes) (bytes / WORD_SIZE_IN_BYTES)
#endif // convertBytesToWords

#ifndef PAGE_SIZE_IN_WORDS
#define PAGE_SIZE_IN_WORDS 4
#endif // PAGE_SIZE_IN_WORDS

#ifndef convertWordsToPages
#define convertWordsToPages(words) (words / PAGE_SIZE_IN_WORDS)
#endif // convertWordsToPages

#ifndef convertBytesToPages
#define convertBytesToPages(bytes) (convertWordsToPages(convertBytesToWords(bytes)))
#endif // convertBytesToPages

enum class SystemMode
{
	KERNEL,
	USER
};

enum class ProgramSection
{
	NONE,
	TEXT, // (a.k.a. JOB or EXECUTABLE section)
	DATA
};

/*
https://en.wikipedia.org/wiki/List_of_numeral_systems#Standard_positional_numeral_systems
*/
enum class NumberSystem
{
	BINARY = 2,
	//OCTAL = 8,
	DECIMAL = 10,
	HEXADECIMAL = 16
	//DUOTRIGESIMAL = 32,
	//HEXATRIGESIMAL = 36
};

enum class ByteOrder
{
	LITTLE_ENDIAN = 0,
	//MIDDLE_ENDIAN,
	BIG_ENDIAN
};

// TODO:: only allow certain class types for pointerToString()
template<class T>
std::string pointerToString(T *aPointer, std::string indent = "")
{
	std::string retval;

	if (aPointer == NULL)
	{
		retval.append(indent);
		retval.append("NULL");
	}
	else
	{
		retval.append(aPointer->toString(indent));
	}

	return retval;
}

/*
	Assuming (ByteOrder::BIG_ENDIAN)...

	<INSTRUCTION_TYPE>		<OPCODE>	<...>
	(2 bits)				(6 bits)	(24 bits)

	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	BINARY		DECIMAL		FORMAT of <...>(24 bits)
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	00			0			arithmetic or register transfer
							<S_REG1>	<S_REG2>	<D_REG>		<UNUSED>
							(4 bits)	(4 bits)	(4 bits)	(12 bits)

	01			1			conditional branch and immediate
							<B_REG>		<D_REG>		<ADDRESS>
							(4 bits)	(4 bits)	(16 bits)

	10			2			unconditional jump
							<ADDRESS>
							(24 bits)

	11			3			input and output
							<REG1>		<REG2>		<ADDRESS>
							(4 bits)	(4 bits)	(16 bits)
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
enum class InstructionType
{
	ARITHMETIC_OR_REGISTER_TRANSFER,
	CONDITIONAL_BRANCH_AND_IMMEDIATE,
	UNCONDITIONAL_JUMP,
	INPUT_AND_OUTPUT
};

std::string instructionTypeToString(InstructionType instructionType)
{
	std::string retval;

	switch (instructionType)
	{
	case InstructionType::ARITHMETIC_OR_REGISTER_TRANSFER:
		retval = "ARITHMETIC_OR_REGISTER_TRANSFER";
		break;
	case InstructionType::CONDITIONAL_BRANCH_AND_IMMEDIATE:
		retval = "CONDITIONAL_BRANCH_AND_IMMEDIATE";
		break;
	case InstructionType::UNCONDITIONAL_JUMP:
		retval = "UNCONDITIONAL_JUMP";
		break;
	case InstructionType::INPUT_AND_OUTPUT:
		retval = "INPUT_AND_OUTPUT";
		break;
	default:
		retval = std::to_string(static_cast<int>(instructionType));
		break;
	}

	return retval;
}

/*
	BINARY		HEX 	INSTRUCTION		DESCRIPTION
	000000		00		RD				reads content of input buffer into a accumulator
	000001		01		WR				writes content of accumulator into output buffer
	000010		02		ST				stores content of a register into an address
	000011		03		LW				loads content of an address into a register
	000100		04		MOV				transfers content of one register into another
	000101		05		ADD				adds content of two source registers (S-reg) into a destination register (D-reg)
	000110		06		SUB				subtracts content of two source registers (S-reg) into a destination register (D-reg)
	000111		07		MUL				multiplies content of two source registers (S-reg) into a destination register (D-reg)
	001000		08		DIV				divides content of two source registers (S-reg) into a destination register (D-reg)
	001001		09		AND				logical AND of two source registers (S-reg) into a destination register (D-reg)
	001010		0A		OR				logical OR of two source registers (S-reg) into a destination register (D-reg)
	001011		0B		MOVI			transfers address/data directly into a register
	001100		0C		ADDI			adds a data value directly to the content of a register
	001101		0D		MULI			multiplies a data value directly with the content of a register
	001110		0E		DIVI			divides a data value directly to the content of a register
	001111		0F		LDI				loads a data/address directly to the content of a register
	010000		10		SLT				sets the destination register (D-reg) to 1 if first source register (S-reg) is less than the base register (B-reg); 0 otherwise
	010001		11		SLTI			sets the destination register (D-reg) to 1 if first source register (S-reg) is less than a data; 0 otherwise
	010010		12		HLT				logical end of program
	010011		13		NOP				does nothing and moves to next instruction
	010100		14		JMP				jumps to a specified location
	010101		15		BEQ				branches to an address when content of base register (B-reg) == destination register (D-reg)
	010110		16		BNE				branches to an address when content of base register (B-reg) != destination register (D-reg)
	010111		17		BEZ				branches to an address when content of base register (B-reg) == 0
	011000		18		BNZ				branches to an address when content of base register (B-reg) != 0
	011001		19		BGZ				branches to an address when content of base register (B-reg) >  0
	011010		1A		BLZ				branches to an address when content of base register (B-reg) <  0
*/
enum class OpCode
{
	RD,
	WR,
	ST,
	LW,
	MOV,
	ADD,
	SUB,
	MUL,
	DIV,
	AND,
	OR,
	MOVI,
	ADDI,
	MULI,
	DIVI,
	LDI,
	SLT,
	SLTI,
	HLT,
	NOP,
	JMP,
	BEQ,
	BNE,
	BEZ,
	BNZ,
	BGZ,
	BLZ
};

std::string opCodeToString(OpCode opCode)
{
	std::string retval;

	switch (opCode)
	{
	case OpCode::RD:
		retval = "RD";
		break;
	case OpCode::WR:
		retval = "WR";
		break;
	case OpCode::ST:
		retval = "ST";
		break;
	case OpCode::LW:
		retval = "LW";
		break;
	case OpCode::MOV:
		retval = "MOV";
		break;
	case OpCode::ADD:
		retval = "ADD";
		break;
	case OpCode::SUB:
		retval = "SUB";
		break;
	case OpCode::MUL:
		retval = "MUL";
		break;
	case OpCode::DIV:
		retval = "DIV";
		break;
	case OpCode::AND:
		retval = "AND";
		break;
	case OpCode::OR:
		retval = "OR";
		break;
	case OpCode::MOVI:
		retval = "MOVI";
		break;
	case OpCode::ADDI:
		retval = "ADDI";
		break;
	case OpCode::MULI:
		retval = "MULI";
		break;
	case OpCode::DIVI:
		retval = "DIVI";
		break;
	case OpCode::LDI:
		retval = "LDI";
		break;
	case OpCode::SLT:
		retval = "SLT";
		break;
	case OpCode::SLTI:
		retval = "SLTI";
		break;
	case OpCode::HLT:
		retval = "HLT";
		break;
	case OpCode::NOP:
		retval = "NOP";
		break;
	case OpCode::JMP:
		retval = "JMP";
		break;
	case OpCode::BEQ:
		retval = "BEQ";
		break;
	case OpCode::BNE:
		retval = "BNE";
		break;
	case OpCode::BEZ:
		retval = "BEZ";
		break;
	case OpCode::BNZ:
		retval = "BNZ";
		break;
	case OpCode::BGZ:
		retval = "BGZ";
		break;
	case OpCode::BLZ:
		retval = "BLZ";
		break;
	default:
		retval = std::to_string(static_cast<int>(opCode));
		break;
	}

	return retval;
}

/*
TERMINATED		the process has finished execution
NEW				the process is being created
READY			the process is waiting to be assigned to a processor
RUNNING			instructions are being executed
BLOCKED			the process is waiting for some event to occur
*/
enum class ProcessStatus
{
	TERMINATED = -1,
	NEW,
	READY,
	RUNNING,
	BLOCKED
};

std::string processStatusToString(ProcessStatus processStatus)
{
	std::string retval;

	switch (processStatus)
	{
	case ProcessStatus::TERMINATED:
		retval = "TERMINATED";
		break;
	case ProcessStatus::NEW:
		retval = "NEW";
		break;
	case ProcessStatus::READY:
		retval = "READY";
		break;
	case ProcessStatus::RUNNING:
		retval = "RUNNING";
		break;
	case ProcessStatus::BLOCKED:
		retval = "BLOCKED";
		break;
	default:
		retval = std::to_string(static_cast<int>(processStatus));
		break;
	}

	return retval;
}

// TODO:: verify type T is a number (i.e. double, float, int, short, ...)
// TODO:: change vector parameter type by adding template for list type
template <typename T>
static T calcAverage(std::vector<T> values)
{
	T sum = 0;

	for each (T item in values)
	{
		sum += item;
	}

	return (sum / values.size());
}

std::vector<std::string> tokenize(std::string originalString, char* delimeters = " ")
{
	std::vector<std::string> token_list;
	char* cString = new char[originalString.length() + 1];
	strcpy(cString, originalString.c_str());
	char* token;

	// get first token
	token = strtok(cString, delimeters);

	while (token != NULL)
	{
		token_list.push_back(std::string(token));
		token = strtok(NULL, delimeters);
	}

	delete[] cString;

	return token_list;
}

bool isBinary(std::string inputNumber)
{
	if (boost::iequals(inputNumber, NOT_A_NUMBER))
		return false;

	for (std::string::size_type i = 0; i < inputNumber.size(); ++i)
	{
		if (inputNumber[i] != '0' && inputNumber[i] != '1')
			return false;
	}

	return true;
}

bool isDecimal(std::string inputNumber)
{
	if (boost::iequals(inputNumber, NOT_A_NUMBER))
		return false;

	for (std::string::size_type i = 0; i < inputNumber.size(); ++i)
	{
		if (isdigit(inputNumber[i]) == false)
			return false;
	}

	return true;
}

bool isHex(std::string inputNumber)
{
	if (boost::iequals(inputNumber, NOT_A_NUMBER))
		return false;

	char currentChar;

	for (std::string::size_type i = 0; i < inputNumber.size(); ++i)
	{
		currentChar = toupper(inputNumber[i]);

		if (isdigit(currentChar) == false
			&& currentChar != 'A'
			&& currentChar != 'B'
			&& currentChar != 'C'
			&& currentChar != 'D'
			&& currentChar != 'E'
			&& currentChar != 'F')
			return false;
	}

	return true;
}

// TODO:: modify val
int val(char c)
{
	char uc = toupper(c);

	if (uc >= '0' && uc <= '9')
		return (int)uc - '0';
	else
		return (int)uc - 'A' + 10;
}

std::string toDecimal(NumberSystem srcBase, ByteOrder endianess, std::string inputNumber)
{
	if (boost::iequals(inputNumber, NOT_A_NUMBER))
	{
		std::cerr << "ERROR:: input is not a number" << std::endl;
		return inputNumber;
	}

	const int base = static_cast<int>(srcBase);
	unsigned long long power = 1;
	unsigned long long decimalNumber = 0;

	switch (srcBase)
	{
	case NumberSystem::BINARY:
		if (isBinary(inputNumber))
		{
			if (inputNumber.size() > WORD_SIZE_IN_BITS)
			{
				std::cerr << "ERROR:: BINARY number out of bounds" << std::endl;
				return NOT_A_NUMBER;
			}
		}
		else
		{
			std::cerr << "ERROR:: source number system specified is BINARY but input number contains non-BINARY characters" << std::endl;
			return NOT_A_NUMBER;
		}
		break;

	case NumberSystem::DECIMAL:
		break;

	case NumberSystem::HEXADECIMAL:
		if (isHex(inputNumber))
		{
			if (inputNumber.size() > (WORD_SIZE_IN_BYTES * 2))
			{
				std::cerr << "ERROR:: HEXADECIMAL number out of bounds" << std::endl;
				return NOT_A_NUMBER;
			}
		}
		else
		{
			std::cerr << "ERROR:: source number system specified is HEXADECIMAL but input number contains non-HEXADECIMAL characters" << std::endl;
			return NOT_A_NUMBER;
		}
		break;

	default:
		break;
	}

	switch (endianess)
	{
	case ByteOrder::LITTLE_ENDIAN:
		// least significant byte stored first
		for (int i = 0; i < inputNumber.size(); ++i)
		{
			decimalNumber += val(inputNumber[i]) * power;
			power *= base;
		}
		break;

	case ByteOrder::BIG_ENDIAN:
		// most significant byte stored first
		for (int i = inputNumber.size() - 1; i >= 0; --i)
		{
			decimalNumber += val(inputNumber[i]) * power;
			power *= base;
		}
		break;

	default:
		break;
	}

	return std::to_string(decimalNumber);
}

// TODO:: modify reVal
char reVal(int num)
{
	if (num >= 0 && num <= 9)
		return (char)(num + '0');
	else
		return (char)(num - 10 + 'A');
}

std::string fromDecimal(NumberSystem destBase, ByteOrder endianess, unsigned long long inputNumber)
{
	int base = (int)destBase;
	std::string newNumber = "";

	while (inputNumber > 0)
	{
		newNumber += reVal(inputNumber % base);
		inputNumber /= base;
	}

	switch (endianess)
	{
	case ByteOrder::LITTLE_ENDIAN:
		// least significant byte stored first
		break;

	case ByteOrder::BIG_ENDIAN:
		// most significant byte stored first
		std::reverse(newNumber.begin(), newNumber.end());
		break;

	default:
		break;
	}

	switch (destBase)
	{
	case NumberSystem::BINARY:
		if (isBinary(newNumber) == false)
		{
			std::cerr << "ERROR:: source number system specified is BINARY but input number contains non-BINARY characters" << std::endl;
			return NOT_A_NUMBER;
		}
		break;

	case NumberSystem::DECIMAL:
		break;

	case NumberSystem::HEXADECIMAL:
		if (isHex(newNumber) == false)
		{
			std::cerr << "ERROR:: source number system specified is HEXADECIMAL but input number contains non-HEXADECIMAL characters" << std::endl;
			return NOT_A_NUMBER;
		}
		break;

	default:
		break;
	}

	return newNumber;
}

// TODO:: go from binary directly to hex and vice versa
// TODO:: use std::vector<bool> OR bitset instead of std::string for binary numbers

std::string changeBase(NumberSystem srcBase, NumberSystem destBase, ByteOrder endianess,
	std::string inputNumber, bool useNumberSystemPrefix = false, bool useLeadingZeros = false)
{
	if (srcBase == destBase)
		return inputNumber;

	std::string trimmedInputNumber = inputNumber;
	std::string outputNumber;
	std::string newNumber;
	
	// put number system prefix on output string
	// (e.g. prefix '0x' for hex value 'FFFFFF' => '0xFFFFFF')
	if (useNumberSystemPrefix)
	{
		switch (destBase)
		{
		case NumberSystem::BINARY:
			outputNumber = BINARY_PREFIX;
			break;

		case NumberSystem::HEXADECIMAL:
			outputNumber = HEXADECIMAL_PREFIX;
			break;

		default:
			break;
		}
	}

	// 1. remove number system prefix from input number (if it exists)
	// 2. verify input number characters are 'legal' given the specified number system 'src'
	//    (e.g. binary numbers should only have chars '1' or '0')
	switch (srcBase)
	{
	case NumberSystem::BINARY:
		// ---------------------------------------------------------
		if (boost::iequals(inputNumber.substr(0, 2), BINARY_PREFIX))
			trimmedInputNumber = inputNumber.substr(2);

		if (isBinary(trimmedInputNumber) == false)
		{
			std::cerr << "ERROR:: source number system specified is BINARY but input number contains non-BINARY characters" << std::endl;
			return NOT_A_NUMBER;
		}

		newNumber = toDecimal(srcBase, endianess, trimmedInputNumber);

		if (isDecimal(newNumber))
			newNumber = fromDecimal(destBase, endianess, std::stoull(newNumber));
		// ---------------------------------------------------------
		break;

	case NumberSystem::DECIMAL:
		// ---------------------------------------------------------
		if (isDecimal(trimmedInputNumber) == false)
		{
			std::cerr << "ERROR:: source number system specified is DECIMAL but input number contains non-DECIMAL characters" << std::endl;
			return NOT_A_NUMBER;
		}

		if (isDecimal(trimmedInputNumber))
			newNumber = fromDecimal(destBase, endianess, std::stoull(trimmedInputNumber));
		// ---------------------------------------------------------
		break;

	case NumberSystem::HEXADECIMAL:
		// ---------------------------------------------------------
		if (boost::iequals(inputNumber.substr(0, 2), HEXADECIMAL_PREFIX))
			trimmedInputNumber = inputNumber.substr(2);

		if (isHex(trimmedInputNumber) == false)
		{
			std::cerr << "ERROR:: source number system specified is HEXADECIMAL but input number contains non-HEXADECIMAL characters" << std::endl;
			return NOT_A_NUMBER;
		}

		newNumber = toDecimal(srcBase, endianess, trimmedInputNumber);
		
		if (isDecimal(newNumber))
			newNumber = fromDecimal(destBase, endianess, std::stoull(newNumber));
		// ---------------------------------------------------------
		break;

	default:
		break;
	}

	switch (destBase)
	{
	case NumberSystem::BINARY:
		// not necessary cuz using bitset
		/*
		if (useLeadingZeros && isBinary(newNumber))
		{
			while (newNumber.size() < (WORD_SIZE_IN_BITS))
			{
				if (endianess == ByteOrder::BIG_ENDIAN)
				{
					newNumber = "0" + newNumber;
				}
				else
				{
					newNumber = newNumber + "0";
				}
			}
		}
		*/
		break;

	case NumberSystem::DECIMAL:
		if (useLeadingZeros && isDecimal(newNumber))
		{
			// TODO:: changeBase:: pad decimal number with leading zeros
			// maybe use setfill('0') & setw()
		}
		break;

	case NumberSystem::HEXADECIMAL:
		if (useLeadingZeros && isHex(newNumber))
		{
			while (newNumber.size() < (WORD_SIZE_IN_BYTES * 2))
			{
				if (endianess == ByteOrder::BIG_ENDIAN)
				{
					newNumber = "0" + newNumber;
				}
				else
				{
					newNumber = newNumber + "0";
				}
			}
		}

		if (newNumber == "")
		{
			newNumber = "0";
		}
		break;

	default:
		break;
	}
	
	outputNumber.append(newNumber);

	return outputNumber;
}
