#pragma once
#include "memory.h"
#include "memorymanagementpolicy.h"
#include "cachepagemanager.h"

// TODO:: ** Cache

class Cache
{
public:
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY;

	Cache(MemoryManagementPolicy mmp, unsigned long long cacheSizeInBytes) :
		MEMORY_MANAGEMENT_POLICY(mmp)
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cache = new Memory(cacheSizeInBytes);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cachePageManager = new CachePageManager(cacheSizeInBytes);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	~Cache()
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cache == nullptr)
				delete cache;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cachePageManager == nullptr)
				delete cachePageManager;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	// Copy constructor
	Cache(const Cache &rhs) :
		MEMORY_MANAGEMENT_POLICY(rhs.MEMORY_MANAGEMENT_POLICY)
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cache == nullptr)
				cache = new Memory(rhs.getCache());
			else
				*cache = rhs.getCache();
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cachePageManager == nullptr)
				cachePageManager = new CachePageManager(rhs.getCachePageManager());
			else
				*cachePageManager = rhs.getCachePageManager();
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	Cache& operator=(const Cache &rhs)
	{
		// TODO:: Cache:: if MEMORY_MANAGEMENT_POLICY != rhs.MEMORY_MANAGEMENT_POLICY ???

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cache == nullptr)
				cache = new Memory(rhs.getCache());
			else
				*cache = rhs.getCache();
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cachePageManager == nullptr)
				cachePageManager = new CachePageManager(rhs.getCachePageManager());
			else
				*cachePageManager = rhs.getCachePageManager();
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return *this;
	}

	Memory getCache() const
	{
		Memory nullmemory(0);

		if (cache != nullptr)
			return *cache;
		else
			return nullmemory;
	}

	CachePageManager getCachePageManager() const
	{
		CachePageManager nullmemory(0);

		if (cachePageManager != nullptr)
			return *cachePageManager;
		else
			return nullmemory;
	}

private:
	Memory *cache = nullptr;
	CachePageManager *cachePageManager = nullptr;
};
