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

// TODO:: combine CachePageManager with PageManager
class CachePageManager
{
public:
	CachePageManager(unsigned long long cacheSizeInBytes)
	{
		cache = new std::vector<Page>(convertBytesToPages(cacheSizeInBytes));
	}

	~CachePageManager()
	{
		if (cache != NULL)
			delete cache;
	}

	// Copy constructor
	CachePageManager(const CachePageManager &rhs)
	{
		if (cache == NULL)
			cache = new std::vector<Page>(rhs.getPages());
		else
			*cache = rhs.getPages();
	}

	CachePageManager& operator=(const CachePageManager &rhs)
	{
		if (cache == NULL)
			cache = new std::vector<Page>(rhs.getPages());
		else
			*cache = rhs.getPages();

		return *this;
	}

	std::vector<Page> getPages() const
	{
		std::vector<Page> nullmemory;

		if (cache != NULL)
			return *cache;
		else
			return nullmemory;
	}

	// @return std::vector<int>, indexes of free pages
	std::vector<int> getFreePagePool(MemoryType memoryType)
	{
		std::vector<int> freePages;

		for (size_t i = 0; i < cache->size(); i++)
		{
			if (cache->at(i).isFree())
				freePages.push_back(i);
		}

		return freePages;
	}

	// TODO:: CachePageManager:: swapPage()
	// TODO:: CachePageManager:: replacePage()

	Word readWord(PageAddress pageAddress) const
	{
		return cache->at(pageAddress.pageNumber).read(pageAddress.offset);
	}

	void writeWord(PageAddress pageAddress, Word newValue)
	{
		cache->at(pageAddress.pageNumber).read(pageAddress.offset) = newValue;
	}

	Page readPage(int pageIndex) const
	{
		return cache->at(pageIndex);
	}

	void writePage(int pageIndex, Page newValue)
	{
		cache->at(pageIndex) = newValue;
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
		retval.append("Cache:\n");
		retval.append(Page::vectorToString(cache, indent, numberSystem, usePrefix, useLeadingZeros));

		return retval;
	}

private:
	std::vector<Page> *cache;
};
