#pragma once

#include "ancillary.h"
#include "decodedinfo.h"
#include "memory.h"
#include "mmu.h"
#include "safequeue.h"
#include "syscall.h"
#include "word.h"
#include <mutex>
#include <string>
#include <thread>

// in bits
#define INSTRUCTION_TYPE_LENGTH 2
#define OPCODE_LENGTH 6
#define REGISTER_LENGTH 4

// TODO:: ** Processor

// TODO:: Processor:: create internal readyQueue for each processor?
// TODO:: Processor:: use paging with cache? create internal PageManager?

/*
	executes the program in the simulated RAM.

	if a program/job is interrupted while on the CPU, the interrupt
	is serviced while the job is suspended. After the service is done,
	the job must be returned to the 'ready-state', at some point in time.

	cpu must check every memory access generated in 'user-mode'
	to be sure it is between the base and limit registers for that user
*/
class Processor
{
public:
	Processor(
		MemoryManagementUnit *mem, 
		SafeQueue<ProcessControlBlock> *wq,
		SafeQueue<ProcessControlBlock> *tq,
		bool enableDmaMode,
		unsigned long long cacheSizeInBytes = 0) :
		dmaModeEnabled(enableDmaMode)
	{
		mmu = mem;
		waitQueue = wq;
		terminatedQueue = tq;
		programCounter = 0;

		switch (mmu->MEMORY_MANAGEMENT_POLICY)
		{
		case MemoryManagementPolicy::CONTIGUOUS_DYNAMIC_RELOCATION:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// cache must be able to hold at least 1 word
			cacheEnabled = (convertBytesToWords(cacheSizeInBytes) > 0);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case MemoryManagementPolicy::PAGING:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// cache must be able to hold at least 1 page
			cacheEnabled = (convertBytesToPages(cacheSizeInBytes) > 0);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		default:
			// ERROR
			break;
		}

		if(cacheEnabled)
			cache = new Memory(cacheSizeInBytes);

		if (cpuThread == nullptr)
		{
			cpuThread = new std::thread(&Processor::ComputeOnly, this);
			cpuThread->detach();
		}
	}

	~Processor()
	{
		/*
		if (cpuThread != nullptr)
		{
			if (cpuThread->joinable())
				cpuThread->join();
		}

		if (dmaThread != nullptr)
		{
			if (dmaThread->joinable())
				dmaThread->join();
		}
		*/

		// don't delete memory just stop pointing to it
		if (mmu != nullptr)
			mmu = nullptr;

		if (cacheEnabled)
			if (cache != nullptr)
				delete cache;
	}

	// Copy constructor
	Processor(const Processor &rhs)
	{
		dmaModeEnabled = rhs.isDmaModeEnabled();
		cacheEnabled = rhs.isCacheEnabled();

		if (cacheEnabled)
		{
			if (cache == nullptr)
				cache = new Memory(rhs.getCache());
			else
				*cache = rhs.getCache();
		}
		else
		{
			cache = nullptr;
		}

		mmu = rhs.getMemoryManagementUnit();
		waitQueue = rhs.getWaitQueue();
		terminatedQueue = rhs.getTerminatedQueue();
		runningProcess = rhs.getRunningProcess();
		programCounter = rhs.getProgramCounter();
		cpuRegisters = rhs.getRegisters();

		if (cpuThread == nullptr)
		{
			cpuThread = new std::thread(&Processor::ComputeOnly, this);
			cpuThread->detach();
		}
	}

	Processor& operator=(const Processor &rhs)
	{
		dmaModeEnabled = rhs.isDmaModeEnabled();
		cacheEnabled = rhs.isCacheEnabled();

		if (cacheEnabled)
		{
			if (cache == nullptr)
				cache = new Memory(rhs.getCache());
			else
				*cache = rhs.getCache();
		}
		else
		{
			cache = nullptr;
		}

		mmu = rhs.getMemoryManagementUnit();
		waitQueue = rhs.getWaitQueue();
		terminatedQueue = rhs.getTerminatedQueue();
		runningProcess = rhs.getRunningProcess();
		programCounter = rhs.getProgramCounter();
		cpuRegisters = rhs.getRegisters();

		if (cpuThread == nullptr)
		{
			cpuThread = new std::thread(&Processor::ComputeOnly, this);
			cpuThread->detach();
		}

		return *this;
	}

	bool isIdle()
	{
		return runningProcess == nullptr;
	}

	void runProcess(ProcessControlBlock newProcess)
	{
		if (runningProcess == nullptr)
			runningProcess = new ProcessControlBlock(newProcess);

		//ComputeOnly();

		/*if(cpuThread == nullptr)
			cpuThread = new std::thread(&Processor::ComputeOnly, this);*/

		//if (dmaThread == nullptr)
		//dmaThread = new std::thread(&Processor::DMA, this);
	}

	ProcessControlBlock stopRunningProcess()
	{
		ProcessControlBlock retval;

		if (runningProcess != nullptr)
		{
			retval = *runningProcess;
			delete runningProcess;

			/*
			if (cpuThread != nullptr)
			{
				if (cpuThread->joinable())
				{
					cpuThread->join();
					cpuThread = nullptr;
				}
			}

			if (dmaThread != nullptr)
			{
				if (dmaThread->joinable())
				{
					dmaThread->join();
					dmaThread = nullptr;
				}
			}
			*/
		}
		else
		{
			// ERROR
			// cannot stop a process that doesn't exist
		}

		return retval;
	}

	bool isDmaModeEnabled() const
	{
		return dmaModeEnabled;
	}

	bool isCacheEnabled() const
	{
		return cacheEnabled;
	}

	Memory getCache() const
	{
		Memory nullmemory(0);

		if (cacheEnabled && cache != nullptr)
			return *cache;
		else
			return nullmemory;
	}

	MemoryManagementUnit* getMemoryManagementUnit() const
	{
		return mmu;
	}

	SafeQueue<ProcessControlBlock>* getWaitQueue() const
	{
		return waitQueue;
	}

	SafeQueue<ProcessControlBlock>* getTerminatedQueue() const
	{
		return terminatedQueue;
	}

	ProcessControlBlock* getRunningProcess() const
	{
		return runningProcess;
	}

	Word getProgramCounter() const
	{
		return programCounter;
	}

	Registers getRegisters() const
	{
		return cpuRegisters;
	}

	// TODO:: Processor:: saveProcessContext()
	ProcessContext saveProcessContext() const
	{
		ProcessContext retval;

		retval.programCounter = programCounter;
		retval.registers = cpuRegisters;

		return retval;
	}

	// TODO:: Processor:: loadProcessContext()
	void loadProcessContext(ProcessContext processContext)
	{
		programCounter = processContext.programCounter;
		cpuRegisters = processContext.registers;
	}

	std::string toString(std::string indent = "")
	{
		std::string retval = "";

		retval.append("\n");
		retval.append(indent);
		retval.append("########################################################################################################################################################################################################################################");
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("Running Process...");
		retval.append("\n");
		retval.append(runningProcess->toString(indent));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("CPU Program Counter:\t");
		retval.append(programCounter.toHexString(true, true));
		retval.append("\n");

		retval.append("\n");
		retval.append(indent);
		retval.append("CPU Registers:\t");
		retval.append(cpuRegisters.toString(indent));
		retval.append("\n");

		if (cacheEnabled && cache != nullptr)
		{
			retval.append("\n");
			retval.append(indent);
			retval.append("CPU Cache:\t");
			retval.append(cache->toString(indent));
			retval.append("\n");
		}

		retval.append("\n");
		retval.append(indent);
		retval.append("########################################################################################################################################################################################################################################");
		retval.append("\n");

		return retval;
	}

private:
	bool dmaModeEnabled;
	bool cacheEnabled;
	std::thread *cpuThread = nullptr;
	std::thread *dmaThread = nullptr;
	Memory *cache = nullptr;
	MemoryManagementUnit *mmu = nullptr;
	SafeQueue<ProcessControlBlock> *waitQueue = nullptr;
	SafeQueue<ProcessControlBlock> *terminatedQueue = nullptr;
	ProcessControlBlock *runningProcess = nullptr;
	Word programCounter;
	Registers cpuRegisters;

	// TODO:: Processor:: implement SystemMode (i.e. user/kernel mode)
	//SystemMode systemMode;
	
	/*
		fetches instructions or data from RAM
		depending on the content of the CPU's program counter (PC).

		ON_FETCH ...
		the PC value should point to the next instruction to be fetched.

		calls the effectiveAddress() method to translate the logical address
		to the corresponding absolute address

		supports the decode() method of the CPU

		Parameter(s): the CPU's program counter (PC)
	*/
	Word fetch(Address logicalAddress)
	{
		Word nullword;
		nullword.setLabel("NULL");

		if (isIdle())
			return nullword;

		Word retval;

		if (cacheEnabled)
		{
			// TODO:: Processor:: fetch() from cache
		}
		else
		{
			if (mmu->isAddressInRange(logicalAddress.get(),
				runningProcess->memoryInfo.baseRegister,
				runningProcess->memoryInfo.limitRegister))
			{
				retval = mmu->readWord(MemoryType::RAM,
					mmu->effectiveAddress(
						logicalAddress,
						runningProcess->memoryInfo.baseRegister)
				);
			}
			else
			{
				retval = nullword;
			}
		}

		return retval;
	}

	/*
		completely decode a fetched instruction
		using the different kinds of address translation schemes of the CPU architecture.
		(see info in file: "Instruction Format")

		ON_DECODE ...
		the needed parameters must be loaded into the appropriate registers
		or data structures pertaining to the program/job
		and readied for the execute() method to function properly.
	*/
	DecodedInfo decode(Word instruction)
	{
		Word instruction_be = instruction;
		int offset = 0;

		// TODO:: delete this and use (registers, block of memory, or stack) instead?
		DecodedInfo decodedInfo;

		if (instruction_be.endianess == ByteOrder::LITTLE_ENDIAN)
			instruction_be.endianess = ByteOrder::BIG_ENDIAN;

		std::string binaryInstruction_be = instruction_be.toBinaryString();

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// <INSTRUCTION_TYPE>	<OPCODE>	<...>
		// (2 bits)				(6 bits)	(24 bits)
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		Word instructionTypeWord;
		instructionTypeWord.fromBinaryString(binaryInstruction_be.substr(offset, INSTRUCTION_TYPE_LENGTH));
		decodedInfo.instructionType = static_cast<InstructionType>(instructionTypeWord.toInt());
		offset += INSTRUCTION_TYPE_LENGTH;

		Word opCodeWord;
		opCodeWord.fromBinaryString(binaryInstruction_be.substr(offset, OPCODE_LENGTH));
		decodedInfo.opCode = static_cast<OpCode>(opCodeWord.toInt());
		offset += OPCODE_LENGTH;

		switch (decodedInfo.instructionType)
		{
		case InstructionType::ARITHMETIC_OR_REGISTER_TRANSFER:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// <S_REG1>		<S_REG2>	<D_REG>		<UNUSED>
			// (4 bits)		(4 bits)	(4 bits)	(12 bits)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			decodedInfo.sourceRegister1.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.sourceRegister2.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.destinationRegister.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			// <UNUSED BITS>
			break;

		case InstructionType::CONDITIONAL_BRANCH_AND_IMMEDIATE:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// <B_REG>		<D_REG>		<ADDRESS>
			// (4 bits)		(4 bits)	(16 bits)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			decodedInfo.baseRegister.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.destinationRegister.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.address.fromBinaryString(binaryInstruction_be.substr(offset));
			break;

		case InstructionType::UNCONDITIONAL_JUMP:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// <ADDRESS>
			// (24 bits)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			decodedInfo.address.fromBinaryString(binaryInstruction_be.substr(offset));
			break;

		case InstructionType::INPUT_AND_OUTPUT:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// <REG1>		<REG2>		<ADDRESS>
			// (4 bits)		(4 bits)	(16 bits)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			decodedInfo.register1.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.register2.fromBinaryString(binaryInstruction_be.substr(offset, REGISTER_LENGTH));
			offset += REGISTER_LENGTH;

			decodedInfo.address.fromBinaryString(binaryInstruction_be.substr(offset));
			break;

		default:
			// ERROR
			// instruction type might be NOP; "does nothing and moves to next instruction"
			break;
		}

		// TODO:: store decoded info in registers?

		return decodedInfo;
	}

	/*
		(essentially a switch-loop of the CPU)
		increment the PC value on 'successful' execution of the current instruction.

		Note: only one process can be running on any processor at any instant

		If an I/O operation is done via an interrupt, or due to any other preemptive instruction,
		the job is suspended until the DMA-Channel method completes the read/write operation,
		or the interrupt is serviced.
	*/
	void execute(DecodedInfo decodedInfo)
	{
		// TODO:: execute() ... accumulator working correctly?

		switch (decodedInfo.opCode)
		{
		case OpCode::RD:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// read the content of (Address or Register2) into (Register1)
			// (e.g. reads content of input buffer into a accumulator)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: OpCode::RD ... how to choose where to read content from?

			/*
			// TODO:: OpCode::RD ... move this to DMA

			// read ... R[register1] <== @address
			cpuRegisters.write(
				decodedInfo.register1.toInt(),
				fetch(decodedInfo.address)
			);

			// read ... R[register1] <== R[register2]
			cpuRegisters.write(
				decodedInfo.register1.toInt(),
				cpuRegisters.read(decodedInfo.register2.toInt())
			);

			*/

			runningProcess->numInputOutputOperations++;

			// send pcb to i/o wait queue
			syscall::wait(OpCode::RD, runningProcess, waitQueue);
			break;

		case OpCode::WR:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// write the content of (Register1) into a (Address or Register2)
			// (e.g. writes content of accumulator into output buffer)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: OpCode::WR ... how to choose where to write content?

			/*
			// TODO:: OpCode::WR ... move this to DMA

			// write ... @address <== R[register1]
			mmu->write(
				MemoryType::RAM,
				fetch(decodedInfo.address),
				cpuRegisters.read(decodedInfo.register1.toInt())
			);

			// write ... R[register2] <== R[register1]
			cpuRegisters.write(
				decodedInfo.register2.toInt(),
				cpuRegisters.read(decodedInfo.register1.toInt())
			);

			*/

			runningProcess->numInputOutputOperations++;

			// send pcb to i/o wait queue
			syscall::wait(OpCode::WR, runningProcess, waitQueue);
			break;

		case OpCode::ST:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// stores content of a register into an address
			// write( R[destination_register] ) @ effective_address(base_register, address)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			mmu->writeWord(
				MemoryType::RAM,
				Address(mmu->MEMORY_MANAGEMENT_POLICY, decodedInfo.address),
				cpuRegisters.read(decodedInfo.destinationRegister.toInt())
			);
			break;

		case OpCode::LW:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// loads content of an address into a register
			// R[destination_register] = fetch( address )
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				fetch(Address(mmu->MEMORY_MANAGEMENT_POLICY, decodedInfo.address))
			);
			break;

		case OpCode::MOV:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// transfers content of one register into another
			// dreg = sreg1
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(), 
				cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
			);
			break;

		case OpCode::ADD:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// adds content of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 + sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt()) 
					+ cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::SUB:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// subtracts content of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 - sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
					- cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::MUL:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// multiplies content of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 * sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
					* cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::DIV:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// divides content of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 / sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
					/ cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::AND:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// logical AND of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 & sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
					& cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::OR:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// logical OR of two source registers (S-reg) into a destination register (D-reg)
			// dreg = sreg1 | sreg2;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.sourceRegister1.toInt())
					| cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			);
			break;

		case OpCode::MOVI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// transfers address/data directly into a register
			// if (dreg == "0000")
			// dreg = address // (i.e. data)
			// else
			// dreg = effective_address(base_register, address)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (decodedInfo.destinationRegister == 0)
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					decodedInfo.address
				);
			}
			else
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					fetch(Address(mmu->MEMORY_MANAGEMENT_POLICY, decodedInfo.address))
				);
			}
			break;

		case OpCode::ADDI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// adds a data value directly to the content of a register
			// dreg += address // (i.e. data)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.destinationRegister.toInt()) 
					+ decodedInfo.address)
			);
			break;

		case OpCode::MULI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// multiplies a data value directly with the content of a register
			// dreg *= address // (i.e. data)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.destinationRegister.toInt())
					* decodedInfo.address)
			);
			break;

		case OpCode::DIVI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// divides a data value directly to the content of a register
			// dreg /= address // (i.e. data)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			cpuRegisters.write(
				decodedInfo.destinationRegister.toInt(),
				(cpuRegisters.read(decodedInfo.destinationRegister.toInt())
					/ decodedInfo.address)
			);
			break;

		case OpCode::LDI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// loads a data/address directly to the content of a register
			// if (dreg == "0000")
			// dreg = address // (i.e. data)
			// else
			// dreg = effective_address(base_register, address)
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (decodedInfo.destinationRegister == 0)
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					decodedInfo.address
				);
			}
			else
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					fetch(Address(mmu->MEMORY_MANAGEMENT_POLICY, decodedInfo.address))
				);
			}
			break;

		case OpCode::SLT:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// sets the destination register (D-reg) to 1 if first source register (S-reg) is less than the base register (B-reg); 0 otherwise
			// if (sreg1 < breg)
			// dreg = 1;
			// else
			// dreg = 0;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: OpCode::SLT ... where is 'breg' defined? should 'breg' be 'sreg2'?
			if (cpuRegisters.read(decodedInfo.sourceRegister1.toInt()) 
				< cpuRegisters.read(decodedInfo.sourceRegister2.toInt()))
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					1
				);
			}
			else
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					0
				);
			}
			break;

		case OpCode::SLTI:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// sets the destination register (D-reg) to 1 if first source register (S-reg) is less than a data; 0 otherwise
			// if (sreg1 < data)
			// dreg = 1;
			// else
			// dreg = 0;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: OpCode::SLTI ... i assume sreg1 should be breg. is that right?
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				< decodedInfo.address)
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					1
				);
			}
			else
			{
				cpuRegisters.write(
					decodedInfo.destinationRegister.toInt(),
					0
				);
			}
			break;

		case OpCode::HLT:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// logical end of program
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// TODO:: is 'decodedInfo.address' necessary for 'OpCode::HLT' ?
			syscall::end(runningProcess, mmu, terminatedQueue);
			break;

		case OpCode::NOP:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// does nothing and moves to next instruction
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			break;

		case OpCode::JMP:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// jumps to a specified location
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			break;

		case OpCode::BEQ:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg == dreg) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				== cpuRegisters.read(decodedInfo.destinationRegister.toInt()))
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		case OpCode::BNE:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg != dreg) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				!= cpuRegisters.read(decodedInfo.destinationRegister.toInt()))
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		case OpCode::BEZ:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg == 0) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				== 0)
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		case OpCode::BNZ:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg != 0) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				!= 0)
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		case OpCode::BGZ:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg > 0) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt())
				> 0)
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		case OpCode::BLZ:
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// if (breg < 0) // branch to an address
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (cpuRegisters.read(decodedInfo.baseRegister.toInt()) 
				< 0)
			{
				programCounter = decodedInfo.address / WORD_SIZE_IN_BYTES;
			}
			break;

		default:
			// ERROR
			break;
		}
	}

	// @param ch, channel or DMA controller
	// @param physicalAddress, RAM address of the physical data to be transferred
	// @param buf, starting address of a RAM buffer into/from which the data is to be transferred
	void read(Address physicalAddress, std::vector<Word> *buffer)
	{
		// TODO:: Read()
	}

	// @param ch, channel or DMA controller
	// @param physicalAddress, RAM address of the physical data to be transferred
	// @param buf, starting address of a RAM buffer into/from which the data is to be transferred
	void write(Word physicalAddress, std::vector<Word> *buffer)
	{
		// TODO:: Write()
	}

	// TODO:: DMA()
	void DMA()
	{
		int nextIO = 0;

		while (true)
		{
			ProcessControlBlock ioProcess = waitQueue->pop();

			/*
			switch (type)
			{
			case 0:
				read(ch, next(physicalAddress), buffer[nextIO]);
				break;

			case 1:
				write(ch, next(physicalAddress), buffer[nextIO]);
				break;

			default:
				// ERROR
				break;
			}
			*/

			// assuming 1 word of 4 bytes at a time
			nextIO++;

			// TODO:: signal the main (virtual) CPU to regain the channel/bus ... necessary?
			// signal(ComputeOnly);
		}
	}

	void ComputeOnly()
	{
		while (true)
		{
			if (isIdle())
			{
				// TODO:: ComputeOnly() ... isIdle()
			}
			else
			{
				// fetch instruction at RAM address - mapped PC
				Word instruction = fetch(Address(mmu->MEMORY_MANAGEMENT_POLICY, programCounter));

				// TODO:: Processor:: delete this.. was for testing decode & execute
				//Word instruction;
				//instruction.fromHexString("C0BA0000");

				std::cout << "Fetched Instruction:\t\t"
					<< instruction.toHexString(true, true)
					<< " ( " << instruction.getLabel() << " )"
					<< std::endl << std::endl;

				// part of decoding of the instruction in instr reg (ir),
				// returning the opcode (oc) and a pointer to a list of
				// significant addresses in 'ir' - saved elsewhere
				DecodedInfo decodedInfo = decode(instruction);

				std::cout << "Disassembled Instruction:\t"
					<< decodedInfo.toAssemblyString() << std::endl;

				std::cout << "Decoded Info:" << decodedInfo.toString() << std::endl;

				// ready for next instruction, increase PC by 1 Word
				programCounter += 1;

				std::cout
					<< "++++++++++++++++++++++++++++++++" << std::endl
					<< "BEFORE execute()" << std::endl
					<< "++++++++++++++++++++++++++++++++" << std::endl
					<< toString("\t") << std::endl;

				execute(decodedInfo);

				std::cout
					<< "++++++++++++++++++++++++++++++++" << std::endl
					<< "AFTER execute()" << std::endl
					<< "++++++++++++++++++++++++++++++++" << std::endl
					<< toString("\t") << std::endl;
			}
		}
	}
};
