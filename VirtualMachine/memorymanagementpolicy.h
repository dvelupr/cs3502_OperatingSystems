#pragma once

// all memory management policies will use fixed partition sizes
// PAGING	...		non-contiguous dynamic relocation
enum class MemoryManagementPolicy
{
	// CONTIGUOUS_STATIC_RELOCATION
	CONTIGUOUS_DYNAMIC_RELOCATION,
	//SEGMENTED,
	//DEMAND_PAGING,
	PAGING
};
