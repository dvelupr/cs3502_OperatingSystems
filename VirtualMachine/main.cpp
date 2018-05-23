// ************************************************************
// ************************************************************
//#define TEST_CHANGE_BASE
//#define TEST_WORD
//#define TEST_REGISTERS
//#define TEST_MEMORY
//#define TEST_LOADER
#define TEST_SCHEDULER
//#define TEST_OS_DRIVER
//#define TEST_PHASE_ONE_PART_ONE
//#define TEST_PHASE_ONE_PART_TWO
//#define TEST_PHASE_TWO_STEP_ONE
//#define TEST_PHASE_TWO_STEP_TWO
// ************************************************************
// ************************************************************
#ifdef TEST_CHANGE_BASE
#include "ancillary.h"
#define TAB3 "   "
#define TAB "     "
#endif

#ifdef TEST_WORD
#include "word.h"
#endif

#ifdef TEST_REGISTERS
#include "registers.h"
#endif

#ifdef TEST_MEMORY
#include "mmu.h"
#endif

#ifdef TEST_LOADER
#include "mmu.h"
#include "loader.h"
#endif

#ifdef TEST_SCHEDULER
#include "loader.h"
#include "mmu.h"
#include "scheduler.h"
#endif

#ifdef TEST_OS_DRIVER
#include "driver.h"
#endif

#ifdef TEST_PHASE_ONE_PART_ONE
#include "driver.h"
#endif

#ifdef TEST_PHASE_ONE_PART_TWO
#include "driver.h"
#endif

#ifdef TEST_PHASE_TWO_STEP_ONE
#include "driver.h"
#endif

#ifdef TEST_PHASE_TWO_STEP_TWO
#include "driver.h"
#endif
// ************************************************************
// ************************************************************
#include <iostream>
// ************************************************************
// ************************************************************

int main()
{
#ifdef TEST_CHANGE_BASE
	std::string binaryNumber  = "101001101001";
	std::string decimalNumber = "2039046183";
	std::string hexNumber     = "CD12AB90";

	// =============================================
	// binary to decimal
	std::string btod_le = changeBase(
		NumberSystem::BINARY,
		NumberSystem::DECIMAL,
		ByteOrder::LITTLE_ENDIAN,
		binaryNumber);

	std::string btod_be = changeBase(
		NumberSystem::BINARY,
		NumberSystem::DECIMAL,
		ByteOrder::BIG_ENDIAN,
		binaryNumber);

	// --------------------------------------------
	// binary to hex
	std::string btoh_le = changeBase(
		NumberSystem::BINARY,
		NumberSystem::HEXADECIMAL,
		ByteOrder::LITTLE_ENDIAN,
		binaryNumber);

	std::string btoh_be = changeBase(
		NumberSystem::BINARY,
		NumberSystem::HEXADECIMAL,
		ByteOrder::BIG_ENDIAN,
		binaryNumber);

	// =============================================
	// decimal to binary
	std::string dtob_le = changeBase(
		NumberSystem::DECIMAL,
		NumberSystem::BINARY,
		ByteOrder::LITTLE_ENDIAN,
		decimalNumber);

	std::string dtob_be = changeBase(
		NumberSystem::DECIMAL,
		NumberSystem::BINARY,
		ByteOrder::BIG_ENDIAN,
		decimalNumber);

	// --------------------------------------------
	// decimal to hex
	std::string dtoh_le = changeBase(
		NumberSystem::DECIMAL,
		NumberSystem::HEXADECIMAL,
		ByteOrder::LITTLE_ENDIAN,
		decimalNumber);

	std::string dtoh_be = changeBase(
		NumberSystem::DECIMAL,
		NumberSystem::HEXADECIMAL,
		ByteOrder::BIG_ENDIAN,
		decimalNumber);

	// =============================================
	// hex to binary
	std::string htob_le = changeBase(
		NumberSystem::HEXADECIMAL,
		NumberSystem::BINARY,
		ByteOrder::LITTLE_ENDIAN,
		hexNumber);

	std::string htob_be = changeBase(
		NumberSystem::HEXADECIMAL,
		NumberSystem::BINARY,
		ByteOrder::BIG_ENDIAN,
		hexNumber);

	// --------------------------------------------
	// hex to decimal
	std::string htod_le = changeBase(
		NumberSystem::HEXADECIMAL,
		NumberSystem::DECIMAL,
		ByteOrder::LITTLE_ENDIAN,
		hexNumber);

	std::string htod_be = changeBase(
		NumberSystem::HEXADECIMAL,
		NumberSystem::DECIMAL,
		ByteOrder::BIG_ENDIAN,
		hexNumber);

	// =============================================

	std::cout << "Binary Number = " << binaryNumber << std::endl;
	std::cout << "Decimal Number = " << decimalNumber << std::endl;
	std::cout << "Hex Number = " << hexNumber << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << TAB << "( LITTLE ENDIAN )" << std::endl;
	std::cout << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << "SRC (base)" << TAB << "DEST (base)" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << std::endl;
	std::cout << "BINARY " << TAB3 << "->" << TAB3 << "DECIMAL" << std::endl;
	std::cout << binaryNumber << TAB3 << btod_le << std::endl;
	std::cout << std::endl;
	std::cout << "BINARY " << TAB3 << "->" << TAB3 << "HEX" << std::endl;
	std::cout << binaryNumber << TAB3 << btoh_le << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "DECIMAL" << TAB3 << "->" << TAB3 << "BINARY" << std::endl;
	std::cout << decimalNumber << TAB << dtob_le << std::endl;
	std::cout << std::endl;
	std::cout << "DECIMAL" << TAB3 << "->" << TAB3 << "HEX" << std::endl;
	std::cout << decimalNumber << TAB << dtoh_le << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "HEX    " << TAB3 << "->" << TAB3 << "BINARY" << std::endl;
	std::cout << hexNumber << TAB << "  " << htob_le << std::endl;
	std::cout << std::endl;
	std::cout << "HEX    " << TAB3 << "->" << TAB3 << "DECIMAL" << std::endl;
	std::cout << hexNumber << TAB << "  " << htod_le << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << TAB << "( BIG ENDIAN )" << std::endl;
	std::cout << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << "SRC (base)" << TAB << "DEST (base)" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << std::endl;
	std::cout << "BINARY " << TAB3 << "->" << TAB3 << "DECIMAL" << std::endl;
	std::cout << binaryNumber << TAB3 << btod_be << std::endl;
	std::cout << std::endl;
	std::cout << "BINARY " << TAB3 << "->" << TAB3 << "HEX" << std::endl;
	std::cout << binaryNumber << TAB3 << btoh_be << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "DECIMAL" << TAB3 << "->" << TAB3 << "BINARY" << std::endl;
	std::cout << decimalNumber << TAB << dtob_be << std::endl;
	std::cout << std::endl;
	std::cout << "DECIMAL" << TAB3 << "->" << TAB3 << "HEX" << std::endl;
	std::cout << decimalNumber << TAB << dtoh_be << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "HEX    " << TAB3 << "->" << TAB3 << "BINARY" << std::endl;
	std::cout << hexNumber << TAB << "  " << htob_be << std::endl;
	std::cout << std::endl;
	std::cout << "HEX    " << TAB3 << "->" << TAB3 << "DECIMAL" << std::endl;
	std::cout << hexNumber << TAB << "  " << htod_be << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << std::endl;
#endif

#ifdef TEST_WORD
	Word wordA;
	Word wordB(56);
	Word wordC = wordB;

	wordA = 8;
	wordA.endianess = ByteOrder::BIG_ENDIAN;
	Word wordD(wordA);

	wordC = wordA + wordB;
	wordD *= wordA + wordB;
	wordC += 125612;
	wordC.endianess = ByteOrder::BIG_ENDIAN;

	Word wordE;
	wordE.endianess = ByteOrder::BIG_ENDIAN;
	wordE.fromBinaryString("111100001111011001101");

	Word wordF;
	wordF.endianess = ByteOrder::BIG_ENDIAN;
	wordF.fromHexString("0x00FBF010");

	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word A" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordA.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordA.toString() << std::endl;
	std::cout << "Binary:  " << wordA.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordA.toHexString() << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word B" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordB.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordB.toString() << std::endl;
	std::cout << "Binary:  " << wordB.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordB.toHexString() << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word C" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordC.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordC.toString() << std::endl;
	std::cout << "Binary:  " << wordC.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordC.toHexString() << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word D" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordD.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordD.toString() << std::endl;
	std::cout << "Binary:  " << wordD.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordD.toHexString() << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word E" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordE.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordE.toString() << std::endl;
	std::cout << "Binary:  " << wordE.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordE.toHexString() << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Word F" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Endianess: " << (wordF.endianess == ByteOrder::BIG_ENDIAN ? "BIG_ENDIAN" : "LITTLE_ENDIAN") << std::endl;
	std::cout << "Decimal: " << wordF.toString() << std::endl;
	std::cout << "Binary:  " << wordF.toBinaryString() << std::endl;
	std::cout << "Hex:     " << wordF.toHexString() << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << std::endl;

#endif

#ifdef TEST_REGISTERS
	Registers registers;

	Word val3;
	val3.fromBinaryString("11111111000011");

	Word val4;
	val4.fromHexString("0x000Ca032");

	Word val5("0B11111111111", NumberSystem::BINARY);
	Word val6("0XFFFFFFFF", NumberSystem::HEXADECIMAL);
	Word val7("0XFFFFFFasgasg", NumberSystem::HEXADECIMAL); // garbage data

	registers.write(0, 32);
	registers.write(2, Word("010111001010", NumberSystem::BINARY));
	registers.write(3, val3);
	registers.write(4, val4);
	registers.write(5, val5);
	registers.write(6, val6);
	registers.write(7, val7);

	registers.changeByteOrder(ByteOrder::BIG_ENDIAN);

	std::cout << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Byte Order:\t" << "( Big Endian )" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "\t" << "Registers (decimal):" << registers.toString("\t", NumberSystem::DECIMAL) << std::endl;
	std::cout << "\t" << "Registers (binary):" << registers.toString("\t", NumberSystem::BINARY, false) << std::endl;
	std::cout << "\t" << "Registers (hex):" << registers.toString("\t", NumberSystem::HEXADECIMAL, true, true) << std::endl;
	std::cout << std::endl;

	registers.changeByteOrder(ByteOrder::LITTLE_ENDIAN);

	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Byte Order:\t" << "( Little Endian )" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "\t" << "Registers (decimal):" << registers.toString("\t", NumberSystem::DECIMAL) << std::endl;
	std::cout << "\t" << "Registers (binary):" << registers.toString("\t", NumberSystem::BINARY, false) << std::endl;
	std::cout << "\t" << "Registers (hex):" << registers.toString("\t", NumberSystem::HEXADECIMAL, true, true) << std::endl;
	std::cout << std::endl;
#endif

#ifdef TEST_MEMORY
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	MemoryManagementUnit mmu(MEMORY_MANAGEMENT_POLICY);

	ByteOrder bo = ByteOrder::BIG_ENDIAN;

	std::cout << "Memory (decimal):" << mmu.toString("", -1, NumberSystem::DECIMAL, bo) << std::endl;
	std::cout << "Memory (binary):" << mmu.toString("", -1, NumberSystem::BINARY, bo, false) << std::endl;
	std::cout << "Memory (hex):" << mmu.toString("", -1, NumberSystem::HEXADECIMAL, bo, true, true) << std::endl;
#endif

#ifdef TEST_LOADER
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	MemoryManagementUnit mmu(MEMORY_MANAGEMENT_POLICY);

	SafeQueue<ProcessControlBlock> *jobQueue = new SafeQueue<ProcessControlBlock>();
	Loader loader(&mmu, jobQueue);
	
	loader.load("Program-File.txt");
	/*
	std::cout << std::endl;
	std::cout << "Memory:" << mmu.toString() << std::endl;
	*/
	
	std::cout << ProcessControlBlock::queueToString(*jobQueue) << std::endl;

#endif

#ifdef TEST_SCHEDULER
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const MemoryAllocationPolicy MEMORY_ALLOCATION_POLICY = MemoryAllocationPolicy::FIRST_FIT;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 1;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	MemoryManagementUnit mmu(MEMORY_MANAGEMENT_POLICY, MEMORY_ALLOCATION_POLICY, ramSizeInBytes, diskSizeInBytes);

	SafeQueue<ProcessControlBlock> *jobQueue = new SafeQueue<ProcessControlBlock>();
	ReadyQueue *readyQueue = new ReadyQueue(CPU_SCHEDULING_POLICY);
	SafeQueue<ProcessControlBlock> *waitQueue = new SafeQueue<ProcessControlBlock>();
	SafeQueue<ProcessControlBlock> *terminatedQueue = new SafeQueue<ProcessControlBlock>();

	Loader loader(&mmu, jobQueue);
	std::vector<Processor> *processors = new std::vector<Processor>();

	if (numProcessors <= 0)
	{
		std::cerr << "ERROR:: number of processors cannot be less than one" << std::endl;
		processors->push_back(Processor(&mmu, waitQueue, terminatedQueue, DMA_MODE_ENABLED, cpuCacheSizeInBytes));
	}
	else
	{
		for (int i = 0; i < numProcessors; i++)
		{
			processors->push_back(Processor(&mmu, waitQueue, terminatedQueue, DMA_MODE_ENABLED, cpuCacheSizeInBytes));
		}
	}

	Dispatcher *dispatcher = new Dispatcher(&mmu, processors, readyQueue, PREEMPTING_ENABLED);

	// MAX_READY_QUEUE_SIZE needs to be larger than the number of jobs (for this test)
	// otherwise there will be an infinite loop in the job scheduler
	JobScheduler *jobScheduler = new JobScheduler(
		&mmu, 
		JOB_SCHEDULING_POLICY,
		jobQueue, 
		readyQueue);

	CpuScheduler *cpuScheduler = new CpuScheduler(
		&mmu, 
		CPU_SCHEDULING_POLICY,
		readyQueue,
		dispatcher);

	loader.load("Program-File.txt");

	std::cout << std::endl;
	std::cout << "Memory:" << mmu.toString() << std::endl;
	
	std::cout << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << "JOB QUEUE" << std::endl;
	std::cout << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << std::endl;

	std::cout << ProcessControlBlock::queueToString(*jobQueue) << std::endl;

	while (!jobQueue->empty())
	{
		// select a job from the job pool to be loaded into main memory
		jobScheduler->schedule();
	}

	std::cout << std::endl;
	std::cout << "Memory:" << mmu.toString() << std::endl;
	
	std::cout << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << "READY QUEUE" << std::endl;
	std::cout << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << "****************************************************************************************************************************************************************************************************************************************" << std::endl;
	std::cout << std::endl;
	
	std::cout << readyQueue->toString() << std::endl;

	while (!readyQueue->empty())
	{
		// select a user program from a list of 'ready' jobs for dispatch.
		cpuScheduler->schedule();
		dispatcher->dispatch();
	}

	std::cout << std::endl;
	std::cout << "Memory:" << mmu.toString() << std::endl;

	delete cpuScheduler;
	delete jobScheduler;
	delete dispatcher;
	delete processors;
	delete terminatedQueue;
	delete waitQueue;
	delete readyQueue;
	delete jobQueue;
#endif

#ifdef TEST_OS_DRIVER
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 1;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	Driver osDriver(
		MEMORY_MANAGEMENT_POLICY,
		JOB_SCHEDULING_POLICY,
		CPU_SCHEDULING_POLICY,
		DMA_MODE_ENABLED,
		PREEMPTING_ENABLED,
		numProcessors,
		cpuCacheSizeInBytes,
		ramSizeInBytes,
		diskSizeInBytes
	);

	osDriver.start();
	//osDriver.stop();
#endif

#ifdef TEST_PHASE_ONE_PART_ONE
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 1;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	Driver osDriver(
		MEMORY_MANAGEMENT_POLICY,
		JOB_SCHEDULING_POLICY,
		CPU_SCHEDULING_POLICY,
		DMA_MODE_ENABLED,
		PREEMPTING_ENABLED,
		numProcessors,
		cpuCacheSizeInBytes,
		ramSizeInBytes,
		diskSizeInBytes
	);

	osDriver.start();
	//osDriver.stop();
#endif

#ifdef TEST_PHASE_ONE_PART_TWO
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 4;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	Driver osDriver(
		MEMORY_MANAGEMENT_POLICY,
		JOB_SCHEDULING_POLICY,
		CPU_SCHEDULING_POLICY,
		DMA_MODE_ENABLED,
		PREEMPTING_ENABLED,
		numProcessors,
		cpuCacheSizeInBytes,
		ramSizeInBytes,
		diskSizeInBytes
	);

	osDriver.start();
	//osDriver.stop();
#endif

#ifdef TEST_PHASE_TWO_STEP_ONE
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 1;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	Driver osDriver(
		MEMORY_MANAGEMENT_POLICY,
		JOB_SCHEDULING_POLICY,
		CPU_SCHEDULING_POLICY,
		DMA_MODE_ENABLED,
		PREEMPTING_ENABLED,
		numProcessors,
		cpuCacheSizeInBytes,
		ramSizeInBytes,
		diskSizeInBytes
	);

	osDriver.start();
	//osDriver.stop();
#endif

#ifdef TEST_PHASE_TWO_STEP_TWO
	const MemoryManagementPolicy MEMORY_MANAGEMENT_POLICY = MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION;
	const SchedulingPolicy DEFAULT_SCHEDULING_POLICY = SchedulingPolicy::FIRST_COME_FIRST_SERVED;
	const SchedulingPolicy JOB_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const SchedulingPolicy CPU_SCHEDULING_POLICY = DEFAULT_SCHEDULING_POLICY;
	const bool DMA_MODE_ENABLED = true;
	const bool PREEMPTING_ENABLED = false;

	int numProcessors = 1;
	unsigned long long cpuCacheSizeInBytes = 0;
	unsigned long long ramSizeInBytes = 4096;
	unsigned long long diskSizeInBytes = 8196;

	Driver osDriver(
		MEMORY_MANAGEMENT_POLICY,
		JOB_SCHEDULING_POLICY,
		CPU_SCHEDULING_POLICY,
		DMA_MODE_ENABLED,
		PREEMPTING_ENABLED,
		numProcessors,
		cpuCacheSizeInBytes,
		ramSizeInBytes,
		diskSizeInBytes
	);

	osDriver.start();
	//osDriver.stop();
#endif

	system("PAUSE");
	return 0;
}