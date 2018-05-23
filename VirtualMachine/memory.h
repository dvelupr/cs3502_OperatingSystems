#pragma once

#include "hole.h"
#include "pcb.h"
#include "word.h"
#include <mutex>
#include <thread>
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

class Memory
{
public:
	Memory(unsigned long long memorySizeInBytes)
	{
		// size must be at least 1 word
		if (convertBytesToWords(memorySizeInBytes) > 0)
		{
			// if conversion results in a decimal number; round-up and convert to int
			memory = new std::vector<Word>(static_cast<int>(ceil(convertBytesToWords(memorySizeInBytes))));
			clear();
		}
	}

	~Memory()
	{
		if(memory != nullptr)
			delete memory;
	}

	// Copy constructor
	Memory(const Memory &rhs)
	{
		if (memory == nullptr)
		{
			memory = new std::vector<Word>(rhs.get());
			holes = rhs.getHoles();
		}
		else
		{
			*memory = rhs.get();
			holes = rhs.getHoles();
		}
	}

	Memory& operator=(const Memory &rhs)
	{
		if (memory == nullptr)
		{
			memory = new std::vector<Word>(rhs.get());
			holes = rhs.getHoles();
		}
		else
		{
			*memory = rhs.get();
			holes = rhs.getHoles();
		}

		return *this;
	}

	void clear(bool zeroFill = false)
	{
		if (memory != nullptr)
		{
			if (!empty())
			{
				holes.clear();

				// make a hole that spans (i.e encompasses) the entire memory space
				holes.push_back(Hole(0, memory->size() - 1));

				if (zeroFill)
					std::fill(memory->begin(), memory->end(), Word(0));
			}
		}
	}

	bool empty() const
	{
		if (memory == nullptr)
			return true;

		if (sumHoles() == memory->size())
			return true;

		return false;
	}

	// @return Word, error if == INT8_MAX
	Word read(Word physicalAddress)
	{
		Word data(INT8_MAX);

		if (memory != nullptr)
		{
			int physicalAddressInt = physicalAddress.toInt();

			// verify that the address is in bounds
			if (physicalAddressInt >= 0 && physicalAddressInt < memory->size())
			{
				// check if address is in a hole
				//if (isHole(physicalAddressInt))
					//return Word(0, HOLE_LABEL);

				data = syncRead(physicalAddressInt);
			}
			else
			{
				// ERROR
				// address out of bounds
			}
		}

		return data;
	}

	// @return bool, success if true
	bool write(Word physicalAddress, Word data, bool onlyWriteToHoles = true)
	{
		bool success = false;

		if (memory != nullptr)
		{
			int physicalAddressInt = physicalAddress.toInt();

			// verify that the address is in bounds
			if (physicalAddressInt >= 0 && physicalAddressInt < memory->size())
			{
				if (onlyWriteToHoles)
				{
					// check if address is in a hole
					if (isHole(physicalAddressInt))
					{
						syncWrite(physicalAddressInt, data);
						removeHole(physicalAddressInt);
					}
				}
				else
				{
					syncWrite(physicalAddressInt, data);

					// check if address is in a hole
					if (isHole(physicalAddressInt))
						removeHole(physicalAddressInt);
				}
			}
			else
			{
				// ERROR
				// address out of bounds
			}
		}

		return success;
	}

	int size() const
	{
		if (memory != nullptr)
			return memory->size();
		else
			return 0;
	}

	std::vector<Word> get() const
	{
		std::vector<Word> nullmemory;

		if (memory != nullptr)
			return *memory;
		else
			return nullmemory;
	}

	std::vector<Hole> getHoles() const
	{
		return holes;
	}

	bool isHole(int physicalAddress) const
	{
		for each (Hole hole in holes)
		{
			if (physicalAddress >= hole.start && physicalAddress <= hole.end)
				return true;
		}

		return false;
	}

	bool isHole(int physicalAddressStart, int physicalAddressEnd) const
	{
		if (physicalAddressStart > physicalAddressEnd)
			return false;

		if (physicalAddressStart == physicalAddressEnd)
			return isHole(physicalAddressStart);

		for each (Hole hole in holes)
		{
			if ((physicalAddressStart >= hole.start && physicalAddressStart <= hole.end)
				&& (physicalAddressEnd >= hole.start && physicalAddressEnd <= hole.end))
				return true;
		}

		return false;
	}

	std::string toString(
		std::string indent = "",
		int wordsPerLine = 16,
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		ByteOrder endianess = ByteOrder::BIG_ENDIAN,
		bool usePrefix = true,
		bool useLeadingZeros = true)
	{
		return Word::vectorToString(memory, indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros);
	}

private:
	std::vector<Word> *memory;
	std::vector<Hole> holes;
	std::mutex rw_mutex;
	std::condition_variable rw_condition;
	// TODO:: Memory:: hole_mutex ... necessary?

	// TODO:: Memory:: findHoles() ... necessary?

	// TODO:: Memory:: combineHoles()
	/*
	void combineHoles()
	{

	}
	*/

	// TODO:: Memory:: createHole()

	// @return int, -1 == not found
	int findHole(int physicalAddress)
	{
		int retval = -1;

		for (int i = 0; i < holes.size(); i++)
		{
			if (physicalAddress >= holes[i].start && physicalAddress <= holes[i].end)
			{
				// found
				return i;
			}
		}

		return retval;
	}

	// @return bool, success == true
	bool removeHole(int physicalAddress)
	{
		if (holes.empty())
			return false;

		int i = findHole(physicalAddress);

		if (i < 0)
			return false;

		if (holes[i].size() == 1)
		{
			holes.erase(holes.begin() + i);
			return true;
		}
		else
		{
			return splitHole(i, physicalAddress);
		}
	}

	// @return bool, success == true
	/*
	bool removeHole(int physicalAddressStart, int physicalAddressEnd)
	{
		if (physicalAddressStart > physicalAddressEnd)
			return false;

		if (physicalAddressStart == physicalAddressEnd)
			return removeHole(physicalAddressStart);

		if (holes.empty())
			return false;

		// TODO:: Memory:: removeHole(int, int)
	}
	*/

	// @param holeIndex, index of hole to split
	// @param splitAddress, point at which to split hole
	// @return bool, success == true
	bool splitHole(int holeIndex, int splitAddress)
	{
		// verify splitAddress is in specified hole
		if (splitAddress >= holes[holeIndex].start
			&& splitAddress <= holes[holeIndex].end)
		{
			// hole must be at least of size 2 in order to be split
			if (holes[holeIndex].size() <= 1)
				return false;

			if (holes[holeIndex].size() == 2)
			{
				Hole leftHole(holes[holeIndex].start, holes[holeIndex].start);
				Hole rightHole(holes[holeIndex].end, holes[holeIndex].end);

				holes.emplace(holes.begin() + holeIndex, leftHole);
				holes.emplace(holes.begin() + holeIndex + 1, rightHole);

				return true;
			}
			else
			{
				Hole leftHole(0, 0);
				Hole rightHole(0, 0);

				if ((splitAddress - 1) <= holes[holeIndex].start)
				{
					leftHole.start = holes[holeIndex].start;
					leftHole.end = holes[holeIndex].start;
				}
				else
				{
					leftHole.start = holes[holeIndex].start;
					leftHole.end = splitAddress - 1;
				}

				if (splitAddress + 1 >= holes[holeIndex].end)
				{
					rightHole.start = holes[holeIndex].end;
					rightHole.end = holes[holeIndex].end;
				}
				else
				{
					rightHole.start = splitAddress + 1;
					rightHole.end = holes[holeIndex].end;
				}
				
				holes.emplace(holes.begin() + holeIndex, leftHole);
				holes.emplace(holes.begin() + holeIndex + 1, rightHole);

				return true;
			}
		}

		return false;
	}

	int sumHoles() const
	{
		int sum = 0;

		for each (Hole hole in holes)
			sum += hole.size();

		return sum;
	}

	Word syncRead(int physicalAddressInt)
	{
		// sync reading & writing 
		// (i.e. wait for writing to stop before reading and don't write while reading)
		std::unique_lock<std::mutex> locker(rw_mutex);
		rw_condition.wait(locker);

		// read data
		return memory->at(physicalAddressInt);
	}

	void syncWrite(int physicalAddressInt, Word data)
	{
		// sync reading & writing 
		// (i.e. don't allow reading while writing)
		std::unique_lock<std::mutex> locker(rw_mutex);

		// write data
		memory->at(physicalAddressInt) = data;

		// sync reading & writing 
		// (i.e. allow reading now and notify everyone waiting to read)
		locker.unlock();
		rw_condition.notify_all();
	}
};
