#pragma once

#include "memorytype.h"
#include "page.h"
#include "pageaddress.h"
#include "pagereplacementpolicy.h"
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

#ifndef PAGE_SIZE_IN_WORDS
#define PAGE_SIZE_IN_WORDS 4
#endif // PAGE_SIZE_IN_WORDS

#ifndef convertWordsToPages
#define convertWordsToPages(words) (words / PAGE_SIZE_IN_WORDS)
#endif // convertWordsToPages

#ifndef convertBytesToPages
#define convertBytesToPages(bytes) (convertWordsToPages(convertBytesToWords(bytes)))
#endif // convertBytesToPages

class PageManager
{
public:
	PageManager(
		unsigned long long ramSizeInBytes,
		unsigned long long diskSizeInBytes)
	{
		ram = new std::vector<Page>(convertBytesToPages(ramSizeInBytes));
		disk = new std::vector<Page>(convertBytesToPages(diskSizeInBytes));
	}

	~PageManager()
	{
		if (ram != NULL)
			delete ram;

		if (disk != NULL)
			delete disk;
	}

	// Copy constructor
	PageManager(const PageManager &rhs)
	{
		if (ram == NULL)
			ram = new std::vector<Page>(rhs.getRAM());
		else
			*ram = rhs.getRAM();

		if (disk == NULL)
			disk = new std::vector<Page>(rhs.getDisk());
		else
			*disk = rhs.getDisk();
	}

	PageManager& operator=(const PageManager &rhs)
	{
		if (ram == NULL)
			ram = new std::vector<Page>(rhs.getRAM());
		else
			*ram = rhs.getRAM();

		if (disk == NULL)
			disk = new std::vector<Page>(rhs.getDisk());
		else
			*disk = rhs.getDisk();

		return *this;
	}

	std::vector<Page> getRAM() const
	{
		std::vector<Page> nullmemory;

		if (ram != NULL)
			return *ram;
		else
			return nullmemory;
	}

	std::vector<Page> getDisk() const
	{
		std::vector<Page> nullmemory;

		if (disk != NULL)
			return *disk;
		else
			return nullmemory;
	}

	// @return std::vector<int>, indexes of free pages
	std::vector<int> getFreePagePool(MemoryType memoryType)
	{
		std::vector<int> freePages;

		switch (memoryType)
		{
		case MemoryType::RAM:
			for (size_t i = 0; i < ram->size(); i++)
			{
				if (ram->at(i).isFree())
					freePages.push_back(i);
			}
			break;

		case MemoryType::DISK:
			for (size_t i = 0; i < disk->size(); i++)
			{
				if (disk->at(i).isFree())
					freePages.push_back(i);
			}
			break;

		default:
			// ERROR
			break;
		}

		return freePages;
	}

	// TODO:: PageManager:: swapPage()
	// TODO:: PageManager:: replacePage()

	Word readWord(MemoryType memoryType, PageAddress pageAddress) const
	{
		Word retval;

		switch (memoryType)
		{
		case MemoryType::RAM:
			retval = ram->at(pageAddress.pageNumber).read(pageAddress.offset);
			break;

		case MemoryType::DISK:
			retval = disk->at(pageAddress.pageNumber).read(pageAddress.offset);
			break;

		default:
			// ERROR
			break;
		}

		return retval;
	}

	void writeWord(MemoryType memoryType, PageAddress pageAddress, Word newValue)
	{
		switch (memoryType)
		{
		case MemoryType::RAM:
			ram->at(pageAddress.pageNumber).read(pageAddress.offset) = newValue;
			break;

		case MemoryType::DISK:
			disk->at(pageAddress.pageNumber).read(pageAddress.offset) = newValue;
			break;

		default:
			// ERROR
			break;
		}
	}

	Page readPage(MemoryType memoryType, int pageIndex) const
	{
		Page retval;

		switch (memoryType)
		{
		case MemoryType::RAM:
			retval = ram->at(pageIndex);
			break;

		case MemoryType::DISK:
			retval = disk->at(pageIndex);
			break;

		default:
			// ERROR
			break;
		}

		return retval;
	}

	void writePage(MemoryType memoryType, int pageIndex, Page newValue)
	{
		switch (memoryType)
		{
		case MemoryType::RAM:
			ram->at(pageIndex) = newValue;
			break;

		case MemoryType::DISK:
			disk->at(pageIndex) = newValue;
			break;

		default:
			// ERROR
			break;
		}
	}

	std::string toString(
		std::string indent = "",
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		bool usePrefix = true,
		bool useLeadingZeros = true)
	{
		std::string retval;

		retval.append(indent);
		retval.append("Page Size (in Words):\t\t");
		retval.append(std::to_string(PAGE_SIZE_IN_WORDS));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("_________________________________________");
		retval.append("\n");

		retval.append(indent + "\t");
		retval.append("Disk:\n");
		retval.append(Page::vectorToString(disk, indent, numberSystem, usePrefix, useLeadingZeros));

		retval.append("\n");
		retval.append(indent);
		retval.append("_________________________________________");
		retval.append("\n");

		retval.append(indent + "\t");
		retval.append("RAM:\n");
		retval.append(Page::vectorToString(ram, indent, numberSystem, usePrefix, useLeadingZeros));

		return retval;
	}

private:
	std::vector<Page> *ram;
	std::vector<Page> *disk;
};
