#pragma once

#include "memorymanagementpolicy.h"
#include "pageaddress.h"
#include "word.h"
#include <string>
#include <vector>

class Address
{
public:
	const bool USES_PAGING;

	Address(MemoryManagementPolicy mmp, Word newAddress) :
		USES_PAGING(mmp == MemoryManagementPolicy::PAGING)
	{
		if (USES_PAGING)
		{
			pageAddress = new PageAddress(newAddress);
		}
		else
		{
			address = new Word(newAddress);
		}
	}

	Address(int pageNumber, int offset) :
		USES_PAGING(true)
	{
		pageAddress = new PageAddress(pageNumber, offset);
	}

	~Address()
	{
		if (USES_PAGING)
		{
			if (pageAddress != nullptr)
				delete pageAddress;
		}
		else
		{
			if(address != nullptr)
				delete address;
		}
	}

	// Copy constructor
	Address(const Address &rhs) :
		USES_PAGING(rhs.USES_PAGING)
	{
		if (USES_PAGING)
		{
			if (pageAddress == nullptr)
				pageAddress = new PageAddress(rhs.getPageAddress());
			else
				*pageAddress = rhs.getPageAddress();
		}
		else
		{
			if (address == nullptr)
				address = new Word(rhs.get());
			else
				*address = rhs.get();
		}
	}

	Address& operator=(const Address &rhs)
	{
		// TODO:: Address:: if USES_PAGING != rhs.USES_PAGING ???

		if (USES_PAGING)
		{
			if (pageAddress == nullptr)
				pageAddress = new PageAddress(rhs.getPageAddress());
			else
				*pageAddress = rhs.getPageAddress();
		}
		else
		{
			if (address == nullptr)
				address = new Word(rhs.get());
			else
				*address = rhs.get();
		}

		return *this;
	}

	Word get() const
	{
		Word nullWord;
		nullWord.setLabel("NULL");

		if (address == nullptr)
			return nullWord;
		else
			return (*address);
	}

	// @return bool, true == success
	bool set(Word newWord)
	{
		if (address == nullptr)
		{
			return false; // ERROR
		}
		else
		{
			*address = newWord;
			return true;
		}
	}

	int getPageNumber() const
	{
		if (!USES_PAGING || pageAddress == nullptr)
			return -1; // ERROR

		return pageAddress->pageNumber;
	}

	// @return bool, true == success
	bool setPageNumber(int newPageNumber)
	{
		if (!USES_PAGING || pageAddress == nullptr)
			return false; // ERROR

		pageAddress->pageNumber = newPageNumber;
		return true;
	}

	int getOffset() const
	{
		if (!USES_PAGING || pageAddress == nullptr)
			return -1; // ERROR

		return pageAddress->offset;
	}

	// @return bool, true == success
	bool setOffset(int newOffset)
	{
		if (!USES_PAGING || pageAddress == nullptr)
			return false; // ERROR

		pageAddress->offset = newOffset;
		return true;
	}

	PageAddress getPageAddress() const
	{
		if (!USES_PAGING || pageAddress == nullptr)
			return PageAddress(-1, -1); // ERROR

		return *pageAddress;
	}

	// TODO:: Address:: toString()
	std::string toString(std::string indent = "")
	{
		std::string retval = "";

		//retval.append(indent);
		//retval.append("Address:\n");

		if (USES_PAGING && pageAddress != nullptr)
		{
			retval.append(pageAddress->toString(indent));
		}
		else
		{
			retval.append(indent);
			retval.append(address->toHexString(true, true));
			retval.append("\n");
		}

		return retval;
	}

private:
	PageAddress *pageAddress;
	Word *address;
};
