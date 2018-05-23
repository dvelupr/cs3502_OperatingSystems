#pragma once

#include "ancillary.h"
#include <bitset>
#include <string>
#include <vector>

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

// TODO:: bug when endianess is LITTLE_ENDIAN by default
class Word
{
public:
	ByteOrder endianess;

#if WORD_SIZE_IN_BITS == 32
	uint32_t value;

	Word() :
		value(0u),
		endianess(ByteOrder::BIG_ENDIAN)
	{}

	Word(uint32_t newValue, ByteOrder newByteOrder = ByteOrder::BIG_ENDIAN) :
		value(newValue),
		endianess(newByteOrder)
	{}

	Word(uint32_t newValue, std::string newLabel)
	{
		value = newValue;
		setLabel(newLabel);
	}
#else
	uint64_t value;

	Word() :
		value(0ul),
		endianess(ByteOrder::BIG_ENDIAN)
	{}

	Word(uint64_t newValue, ByteOrder newByteOrder = ByteOrder::BIG_ENDIAN) :
		value(newValue),
		endianess(newByteOrder)
	{}

	Word(uint64_t newValue, std::string newLabel)
	{
		value = newValue;
		setLabel(newLabel);
	}
#endif

	Word(std::string newValue,
		NumberSystem numberSystem,
		ByteOrder newByteOrder = ByteOrder::BIG_ENDIAN) :
		endianess(newByteOrder)
	{
		switch (numberSystem)
		{
		case NumberSystem::BINARY:
			fromBinaryString(newValue);
			break;

		case NumberSystem::DECIMAL:
			fromDecimalString(newValue);
			break;

		case NumberSystem::HEXADECIMAL:
			fromHexString(newValue);
			break;

		default:
			// ERROR
			break;
		}
	}

	~Word()
	{
		deleteLabel();
	}

	// Copy constructor
	Word(const Word &rhs)
	{
		value = rhs.value;
		endianess = rhs.endianess;
		setLabel(rhs.getLabel());
	}

	Word& operator=(const Word &rhs)
	{
		value = rhs.value;
		endianess = rhs.endianess;
		setLabel(rhs.getLabel());

		return *this;
	}

	bool operator==(const Word &rhs) const
	{
		return (endianess == rhs.endianess && value == rhs.value);
	}

	bool operator!=(const Word &rhs) const
	{
		return (endianess != rhs.endianess || value != rhs.value);
	}

	bool operator>(const Word &rhs) const { return value > rhs.value; }
	bool operator<(const Word &rhs) const { return value < rhs.value; }
	bool operator>=(const Word &rhs) const { return value >= rhs.value; }
	bool operator<=(const Word &rhs) const { return value <= rhs.value; }

	bool operator>(const int rhs) const { return value > rhs; }
	bool operator<(const int rhs) const { return value < rhs; }
	bool operator>=(const int rhs) const { return value >= rhs; }
	bool operator<=(const int rhs) const { return value <= rhs; }
	bool operator==(const int rhs) const { return value == rhs; }
	bool operator!=(const int rhs) const { return value != rhs; }

	friend bool operator>(int lhs, const Word &rhs) { return lhs > rhs.value; }
	friend bool operator<(int lhs, const Word &rhs) { return lhs < rhs.value; }
	friend bool operator>=(int lhs, const Word &rhs) { return lhs >= rhs.value; }
	friend bool operator<=(int lhs, const Word &rhs) { return lhs <= rhs.value; }
	friend bool operator==(int lhs, const Word &rhs) { return lhs == rhs.value; }
	friend bool operator!=(int lhs, const Word &rhs) { return lhs != rhs.value; }

	// prefix increment operator
	Word& operator++() { value++; return *this; }

	// postfix increment operator
	Word operator++(int) 
	{
		Word temp = *this;
		++*this;
		return temp;
	}

	// prefix decrement operator
	Word& operator--() { value--; return *this; }

	// postfix decrement operator
	Word operator--(int)
	{
		Word temp = *this;
		--*this;
		return temp;
	}

	Word& operator+=(const Word &rhs)
	{
		value += rhs.value;
		return *this;
	}

	friend Word operator+(Word lhs, const Word &rhs)
	{
		lhs.value += rhs.value;
		return lhs;
	}

	Word& operator-=(const Word &rhs)
	{
		value -= rhs.value;
		return *this;
	}

	friend Word operator-(Word lhs, const Word &rhs)
	{
		lhs.value -= rhs.value;
		return lhs;
	}

	Word& operator*=(const Word &rhs)
	{
		value *= rhs.value;
		return *this;
	}

	friend Word operator*(Word lhs, const Word &rhs)
	{
		lhs.value *= rhs.value;
		return lhs;
	}

	Word& operator/=(const Word &rhs)
	{
		value /= rhs.value;
		return *this;
	}

	friend Word operator/(Word lhs, const Word &rhs)
	{
		lhs.value /= rhs.value;
		return lhs;
	}

	Word& operator&=(const Word &rhs)
	{
		value &= rhs.value;
		return *this;
	}

	friend Word operator&(Word lhs, const Word &rhs)
	{
		lhs.value &= rhs.value;
		return lhs;
	}

	Word& operator|=(const Word &rhs)
	{
		value |= rhs.value;
		return *this;
	}

	friend Word operator|(Word lhs, const Word &rhs)
	{
		lhs.value |= rhs.value;
		return lhs;
	}

	void fromDecimalString(std::string decimalString)
	{
		if (decimalString == "")
			decimalString = "0";

		if (isDecimal(decimalString))
		{
			unsigned long long newValue = stoull(decimalString);
			value = newValue;
		}
		else
		{
			value = 0;
			setLabel("NAN - not a number");
		}
	}

	void fromBinaryString(std::string binaryString)
	{
		std::string decimalString = changeBase(NumberSystem::BINARY, NumberSystem::DECIMAL, endianess, binaryString);

		fromDecimalString(decimalString);
	}

	void fromHexString(std::string hexString)
	{
		std::string decimalString = changeBase(NumberSystem::HEXADECIMAL, NumberSystem::DECIMAL, endianess, hexString);

		if (decimalString == ""
			|| decimalString == "00000000"
			|| decimalString == "0000000000000000")
		{
			decimalString = "0";
		}

		fromDecimalString(decimalString);
	}

	int toInt() const
	{
		// TODO:: convert unsigned to signed int
		// casting from unsigned to signed for values larger than INT_MAX is implementation-defined
		return static_cast<int>(value);
	}

	std::string toString() const
	{
		return std::to_string(value);
	}

	std::string toBinaryString(bool usePrefix = false) const
	{
		std::string retval;

		if (usePrefix)
			retval.append(BINARY_PREFIX);

		retval.append(getBitset(endianess).to_string());

		return retval;
	}

	std::string toHexString(bool usePrefix = false, bool useLeadingZeros = false) const
	{
		return changeBase(NumberSystem::DECIMAL, NumberSystem::HEXADECIMAL, endianess, std::to_string(value), usePrefix, useLeadingZeros);
	}

#if WORD_SIZE_IN_BITS == 32
	 // bitset by default is in BIG_ENDIAN format
	std::bitset<WORD_SIZE_IN_BITS> getBitset(ByteOrder byteOrder) const
	{
		std::bitset<WORD_SIZE_IN_BITS> retval;

		if (byteOrder == ByteOrder::BIG_ENDIAN)
		{
			retval = std::bitset<WORD_SIZE_IN_BITS>(value);
		}
		else if (byteOrder == ByteOrder::LITTLE_ENDIAN)
		{
			// reverse order of bits
			std::string bits = std::bitset<WORD_SIZE_IN_BITS>(value).to_string();
			std::reverse(bits.begin(), bits.end());
			retval = std::bitset<WORD_SIZE_IN_BITS>(bits);
		}
		else
		{
			// ERROR
		}

		return retval;
	}
#else
	// bitset by default is in BIG_ENDIAN format
	std::bitset<64> getBitset(ByteOrder byteOrder) const
	{
		std::bitset<64> retval;

		if (byteOrder == ByteOrder::BIG_ENDIAN)
		{
			retval = std::bitset<64>(value);
		}
		else if (byteOrder == ByteOrder::LITTLE_ENDIAN)
		{
			// reverse order of bits
			std::string bits = std::bitset<64>(value).to_string();
			std::reverse(bits.begin(), bits.end());
			retval = std::bitset<64>(bits);
		}
		else
		{
			// ERROR
		}

		return retval;
	}
#endif

	std::string getLabel() const
	{
		std::string retval;

		if (label == nullptr)
		{
			retval = "NULL";
		}
		else
		{
			retval = *label;
		}

		return retval;
	}

	void setLabel(std::string newLabel)
	{
		if (label == nullptr)
			label = new std::string(newLabel);
		else
			*label = newLabel;
	}

	void deleteLabel()
	{
		if (label != nullptr)
			delete label;
	}

	static std::string vectorToString(
		const std::vector<Word> *wordVector,
		std::string indent = "",
		int wordsPerLine = 32,
		NumberSystem numberSystem = NumberSystem::DECIMAL,
		ByteOrder endianess = ByteOrder::BIG_ENDIAN,
		bool usePrefix = false,
		bool useLeadingZeros = false)
	{
		std::string retval;
		Word tempWord;

		retval.append(indent + "\t");

		if (wordVector != nullptr)
		{
			if (wordVector->empty())
			{
				retval.append("EMPTY");
			}
			else
			{
				switch (numberSystem)
				{
				case NumberSystem::BINARY:
					if (wordsPerLine == 32 || wordsPerLine < 0)
						wordsPerLine = 6;

					for (int i = 0; i < wordVector->size(); i++)
					{
						if (wordVector->at(i).endianess != endianess)
						{
							tempWord = wordVector->at(i);
							tempWord.endianess = endianess;
							retval.append(tempWord.toBinaryString(usePrefix));
						}
						else
						{
							retval.append(wordVector->at(i).toBinaryString(usePrefix));
						}

						if ((i + 1) % wordsPerLine == 0)
						{
							retval.append("\n");
							retval.append(indent + "\t");
						}
						else
						{
							if (i != wordVector->size() - 1)
								retval.append(" , ");
						}
					}
					break;

				case NumberSystem::DECIMAL:
					if (wordsPerLine < 0)
						wordsPerLine = 32;

					for (int i = 0; i < wordVector->size(); i++)
					{
						if (wordVector->at(i).endianess != endianess)
						{
							tempWord = wordVector->at(i);
							tempWord.endianess = endianess;
							retval.append(tempWord.toString());
						}
						else
						{
							retval.append(wordVector->at(i).toString());
						}

						if ((i + 1) % wordsPerLine == 0)
						{
							retval.append("\n");
							retval.append(indent + "\t");
						}
						else
						{
							if (i != wordVector->size() - 1)
								retval.append(" , ");
						}
					}
					break;

				case NumberSystem::HEXADECIMAL:
					if (wordsPerLine == 32 || wordsPerLine < 0)
						wordsPerLine = 16;

					for (int i = 0; i < wordVector->size(); i++)
					{
						if (wordVector->at(i).endianess != endianess)
						{
							tempWord = wordVector->at(i);
							tempWord.endianess = endianess;
							retval.append(tempWord.toHexString(usePrefix, useLeadingZeros));
						}
						else
						{
							retval.append(wordVector->at(i).toHexString(usePrefix, useLeadingZeros));
						}

						if ((i + 1) % wordsPerLine == 0)
						{
							retval.append("\n");
							retval.append(indent + "\t");
						}
						else
						{
							if (i != wordVector->size() - 1)
								retval.append(" , ");
						}
					}
					break;

				default:
					retval.append("ERROR:: Unknown Number System");
					break;
				}
			}
		}
		else
		{
			retval.append("NULL");
		}
		retval.append("\n");

		return retval;
	}

private:
	std::string* label = nullptr;
};
