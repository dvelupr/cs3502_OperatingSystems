#pragma once

#include "address.h"
#include "hole.h"
#include "memory.h"
#include "memoryallocationpolicy.h"
#include "memorymanagementpolicy.h"
#include "memorytype.h"
#include "pagemanager.h"

class MemoryManagementUnit
{
public:
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY;
	const MemoryAllocationPolicy MEMORY_ALLOCATION_POLICY;

	MemoryManagementUnit(
		MemoryManagementPolicy mmp,
		MemoryAllocationPolicy map,
		unsigned long long ramSizeInBytes = 4096,
		unsigned long long diskSizeInBytes = 8196) :
		MEMORY_MANAGEMENT_POLICY(mmp),
		MEMORY_ALLOCATION_POLICY(map)
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			ram = new Memory(ramSizeInBytes);
			disk = new Memory(diskSizeInBytes);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			pageManager = new PageManager(ramSizeInBytes, diskSizeInBytes);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	~MemoryManagementUnit()
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			delete ram;
			delete disk;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			delete pageManager;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	/*
	// TODO:: MMU:: map()
	void map()
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	// TODO:: MMU:: unmap()
	void unmap()
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}
	*/

	bool isAddressInRange(Word address, Word base, Word limit)
	{
		bool retval = false;

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			retval = (address >= base && address < (base + limit));
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: MMU:: isAddressInRange() ... PAGING
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return retval;
	}

	/*
		supports the fetch() and decode() methods
		supports two kinds of address translations - direct and indirect, using the index register.

		Parameter(s): a logical address
		Return: a absolute/physical address that corresponds to the logical address parameter

		------------------------------------------------------------
		direct addressing:			EA = C(base-reg) + D;
		indirect addressing:		EA = C(base-reg) + C(I-reg) + D;
		------------------------------------------------------------
		D is the 16-bit offset or displacement
		base-reg is the base-register value
		C is a 'calculated' offset/address displacement
		I-reg is the index register
		------------------------------------------------------------
	*/
	Address effectiveAddress(
		Address logicalAddress,
		Word baseRegister = Word(0),
		Word indexRegister = Word(-1))
	{
		Address physicalAddress(MEMORY_MANAGEMENT_POLICY, Word(0));

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (indexRegister.toInt() < 0)
			{
				// direct addressing
				physicalAddress.set(baseRegister + logicalAddress.get());
			}
			else
			{
				// indirect addressing
				physicalAddress.set(baseRegister + logicalAddress.get() + indexRegister);
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: MMU:: effectiveAddress() ... PAGING
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return physicalAddress;
	}

	// TODO:: mmu:: readWord() ... take into account process id ... to control reading
	Word readWord(MemoryType memoryType, Address physicalAddress)
	{
		Word data;

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (memoryType)
			{
			case MemoryType::RAM:
				data = ram->read(physicalAddress.get());
				break;

			case MemoryType::DISK:
				data = disk->read(physicalAddress.get());
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			data = pageManager->readWord(memoryType, physicalAddress.getPageAddress());
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return data;
	}

	// TODO:: mmu:: writeWord() ... take into account process id ... to control writing
	void writeWord(MemoryType memoryType, Address physicalAddress, Word data, bool onlyWriteToHoles = true)
	{
		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (memoryType)
			{
			case MemoryType::RAM:
				if (onlyWriteToHoles)
				{
					if (ram->isHole(physicalAddress.get().toInt()))
						ram->write(physicalAddress.get(), data);
				}
				else
				{
					ram->write(physicalAddress.get(), data);
				}
				break;

			case MemoryType::DISK:
				if (onlyWriteToHoles)
				{
					if (disk->isHole(physicalAddress.get().toInt()))
						disk->write(physicalAddress.get(), data);
				}
				else
				{
					disk->write(physicalAddress.get(), data);
				}
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: mmu:: write():: PAGING ... onlyWriteToHoles
			pageManager->writeWord(memoryType, physicalAddress.getPageAddress(), data);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	/*
	// TODO:: MMU:: move()
	void move(MemoryType memoryType, Word sourcePhysicalAddress, int sourceSize, Word destinationPhysicalAddress)
	{
		
	}

	// TODO:: MMU:: compactRAM()
	void compactRAM()
	{
		
	}

	// TODO:: MMU:: defragDisk()
	void defragDisk()
	{
		
	}
	*/

	// onlyWriteToHoles ... verify process isn't being inserted on top of an existing process
	bool canInsertProcess(MemoryType memoryType, ProcessControlBlock pcb, bool onlyWriteToHoles = true)
	{
		bool retval = true;

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (memoryType)
			{
			case MemoryType::RAM:
				if (pcb.memoryInfo.baseRegister < 0
					|| pcb.memoryInfo.limitRegister < 0
					|| pcb.memoryInfo.baseRegister >= ram->size()
					|| (pcb.memoryInfo.baseRegister + pcb.memoryInfo.limitRegister) >= ram->size())
				{
					// ERROR
					// address out of bounds
					return false;
				}

				if (onlyWriteToHoles)
				{
					if (!ram->isHole(pcb.memoryInfo.baseRegister.toInt(), pcb.memoryInfo.baseRegister.toInt() + pcb.memoryInfo.limitRegister.toInt()))
						return false;
				}
				else
				{
					// TODO:: mmu:: canInsertProcess() ... onlyWriteToHoles == false
				}
				break;

			case MemoryType::DISK:
				if (pcb.memoryInfo.baseRegister < 0
					|| pcb.memoryInfo.limitRegister < 0
					|| pcb.memoryInfo.baseRegister >= disk->size()
					|| (pcb.memoryInfo.baseRegister + pcb.memoryInfo.limitRegister) >= disk->size())
				{
					// ERROR
					// address out of bounds
					return false;
				}

				if (onlyWriteToHoles)
				{
					if (!disk->isHole(pcb.memoryInfo.baseRegister.toInt(), pcb.memoryInfo.baseRegister.toInt() + pcb.memoryInfo.limitRegister.toInt()))
						return false;
				}
				else
				{
					// TODO:: mmu:: canInsertProcess() ... onlyWriteToHoles == false
				}
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: MMU:: canInsertProcess() ... PAGING
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return retval;
	}

	// @return int, -1 == no location found
	int findLocationToInsertProcess(MemoryType memoryType, ProcessControlBlock pcb, bool onlyWriteToHoles = true)
	{
		int location = -1;

		// don't relocate process
		if (canInsertProcess(memoryType, pcb, onlyWriteToHoles))
			return pcb.memoryInfo.baseRegister.toInt();

		// used by ... MemoryAllocationPolicy::BEST_FIT
		int minHoleSize = INT_MAX;

		// used by ... MemoryAllocationPolicy::WORST_FIT
		int maxHoleSize = 0;

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (MEMORY_ALLOCATION_POLICY)
			{
			case MemoryAllocationPolicy::BEST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					if (onlyWriteToHoles)
					{
						auto holes = ram->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								// does process fit better in this hole than other holes?
								if (minHoleSize > diffHoleProcess)
								{
									minHoleSize = diffHoleProcess;
									location = holes[i].start;
								}
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				case MemoryType::DISK:
					if (onlyWriteToHoles)
					{
						auto holes = disk->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								// does process fit better in this hole than other holes?
								if (minHoleSize > diffHoleProcess)
								{
									minHoleSize = diffHoleProcess;
									location = holes[i].start;
								}
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			case MemoryAllocationPolicy::FIRST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					if (onlyWriteToHoles)
					{
						auto holes = ram->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								location = holes[i].start;
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				case MemoryType::DISK:
					if (onlyWriteToHoles)
					{
						auto holes = disk->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								location = holes[i].start;
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			case MemoryAllocationPolicy::WORST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					if (onlyWriteToHoles)
					{
						auto holes = ram->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								// does process fit worse in this hole than other holes?
								if (maxHoleSize < diffHoleProcess)
								{
									maxHoleSize = diffHoleProcess;
									location = holes[i].start;
								}
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				case MemoryType::DISK:
					if (onlyWriteToHoles)
					{
						auto holes = disk->getHoles();

						for (int i = 0; i < holes.size(); i++)
						{
							// difference between current hole size and process size
							int diffHoleProcess = (holes[i].size() - pcb.memoryInfo.limitRegister.toInt());

							// does process fit in current hole?
							if (diffHoleProcess > 0)
							{
								// does process fit worse in this hole than other holes?
								if (maxHoleSize < diffHoleProcess)
								{
									maxHoleSize = diffHoleProcess;
									location = holes[i].start;
								}
							}
						}
					}
					else
					{
						// TODO:: mmu:: findLocationToInsertProcess() ... onlyWriteToHoles == false
					}
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: MMU:: findLocationToInsertProcess() ... PAGING

			switch (MEMORY_ALLOCATION_POLICY)
			{
			case MemoryAllocationPolicy::BEST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					break;

				case MemoryType::DISK:
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			case MemoryAllocationPolicy::FIRST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					break;

				case MemoryType::DISK:
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			case MemoryAllocationPolicy::WORST_FIT:
				// #########################################
				switch (memoryType)
				{
				case MemoryType::RAM:
					break;

				case MemoryType::DISK:
					break;

				default:
					// ERROR
					break;
				}
				// #########################################
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		return location;
	}

	// @return bool, success == true
	bool insertProcess(MemoryType memoryType, ProcessControlBlock pcb, bool onlyWriteToHoles = true)
	{
		int i = findLocationToInsertProcess(memoryType, pcb, onlyWriteToHoles);

		if (i < 0)
			return false;

		// relocate process
		Word currentWordAddress = pcb.memoryInfo.baseRegister = i;

		//PageAddress currentPageAddress = destinationPhysicalAddress.getPageAddress();

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (memoryType)
			{
			case MemoryType::RAM:
				// write section... text
				if (pcb.memoryInfo.text.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.text.size(); i++)
					{
						ram->write(currentWordAddress++, pcb.memoryInfo.text[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... data
				if (pcb.memoryInfo.data.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.data.size(); i++)
					{
						ram->write(currentWordAddress++, pcb.memoryInfo.data[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... heap
				if (pcb.memoryInfo.heap.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.heap.size(); i++)
					{
						ram->write(currentWordAddress++, pcb.memoryInfo.heap[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... stack
				if (pcb.memoryInfo.stack.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.stack.size(); i++)
					{
						ram->write(currentWordAddress++, pcb.memoryInfo.stack[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}
				break;

			case MemoryType::DISK:
				// write section... text
				if (pcb.memoryInfo.text.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.text.size(); i++)
					{
						disk->write(currentWordAddress++, pcb.memoryInfo.text[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... data
				if (pcb.memoryInfo.data.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.data.size(); i++)
					{
						disk->write(currentWordAddress++, pcb.memoryInfo.data[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... heap
				if (pcb.memoryInfo.heap.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.heap.size(); i++)
					{
						disk->write(currentWordAddress++, pcb.memoryInfo.heap[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// write section... stack
				if (pcb.memoryInfo.stack.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.stack.size(); i++)
					{
						disk->write(currentWordAddress++, pcb.memoryInfo.stack[i]);
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: MMU:: insertProcess() ... PAGING
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	void removeProcess(MemoryType memoryType, ProcessControlBlock pcb)
	{
		Word currentWordAddress = pcb.memoryInfo.baseRegister;
		//PageAddress currentPageAddress = destinationPhysicalAddress.getPageAddress();

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (memoryType)
			{
			case MemoryType::RAM:
				if (!isAddressInRange(currentWordAddress, pcb.memoryInfo.baseRegister, pcb.memoryInfo.limitRegister)
					|| pcb.memoryInfo.baseRegister >= ram->size()
					|| pcb.memoryInfo.limitRegister >= ram->size())
				{
					// ERROR
					// address out of bounds
					return;
				}

				// delete section... text
				if (pcb.memoryInfo.text.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.text.size(); i++)
					{
						ram->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... data
				if (pcb.memoryInfo.data.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.data.size(); i++)
					{
						ram->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... heap
				if (pcb.memoryInfo.heap.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.heap.size(); i++)
					{
						ram->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... stack
				if (pcb.memoryInfo.stack.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.stack.size(); i++)
					{
						ram->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}
				break;

			case MemoryType::DISK:
				if (!isAddressInRange(currentWordAddress, pcb.memoryInfo.baseRegister, pcb.memoryInfo.limitRegister)
					|| pcb.memoryInfo.baseRegister >= disk->size()
					|| pcb.memoryInfo.limitRegister >= disk->size())
				{
					// ERROR
					// address out of bounds
					return;
				}

				// delete section... text
				if (pcb.memoryInfo.text.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.text.size(); i++)
					{
						disk->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... data
				if (pcb.memoryInfo.data.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.data.size(); i++)
					{
						disk->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... heap
				if (pcb.memoryInfo.heap.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.heap.size(); i++)
					{
						disk->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}

				// delete section... stack
				if (pcb.memoryInfo.stack.empty() == false)
				{
					for (int i = 0; i < pcb.memoryInfo.stack.size(); i++)
					{
						disk->write(currentWordAddress++, Word(0));
					}
				}
				else
				{
					// ERROR
					// section doesn't exist; nothing to write to memory
				}
				break;

			default:
				// ERROR
				break;
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// TODO:: MMU:: removeProcess() ... PAGING
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}
	}

	// TODO:: MMU:: moveProcess()
	// @return bool, success == true
	/*
	bool moveProcess(MemoryType memoryType, Word destinationPhysicalAddress, ProcessControlBlock pcb)
	{

	}
	*/

	std::string toString(
		std::string indent = "",
		int wordsPerLine = 16,
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		ByteOrder endianess = ByteOrder::BIG_ENDIAN,
		bool usePrefix = true,
		bool useLeadingZeros = true)
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append("_________________________________________");
		retval.append("\n");

		switch (MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			retval.append(indent);
			retval.append("Word Size (in Bytes):\t\t");
			retval.append(std::to_string(WORD_SIZE_IN_BYTES));
			retval.append("\n");

			retval.append("\n");
			retval.append(indent);
			retval.append("_________________________________________");
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("Disk:\n");
			retval.append(disk->toString(indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));

			retval.append("\n");
			retval.append(indent);
			retval.append("_________________________________________");
			retval.append("\n");

			retval.append(indent + "\t");
			retval.append("RAM:\n");
			retval.append(ram->toString(indent, wordsPerLine, numberSystem, endianess, usePrefix, useLeadingZeros));
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			retval.append(pageManager->toString(indent, numberSystem, usePrefix, useLeadingZeros));
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		retval.append("\n");
		retval.append(indent);
		retval.append("_________________________________________");
		retval.append("\n");

		return retval;
	}

private:
	Memory *ram;
	Memory *disk;
	PageManager *pageManager;

	// prevent copy-construction or assignment
	MemoryManagementUnit(const MemoryManagementUnit& other) : MEMORY_MANAGEMENT_POLICY(), MEMORY_ALLOCATION_POLICY() {}
	MemoryManagementUnit& operator=(const MemoryManagementUnit& other) {}
};
