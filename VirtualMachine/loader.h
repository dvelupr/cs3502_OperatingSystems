#pragma once

#include "ancillary.h"
#include "mmu.h"
#include "pcb.h"
#include "safequeue.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

class Loader
{
public:
	Loader(MemoryManagementUnit *mem, SafeQueue<ProcessControlBlock> *jq)
	{
		mmu = mem;
		numLoadedJobs = 0;
		jobQueue = jq;
	}

	~Loader()
	{
		if (mmu != NULL)
			mmu = NULL;
	}

	void load(std::string filename)
	{
		// opens (once at the start) the 'program-file' and performs the loading process.

		// load user programs, or jobs, which are already assembled
		// (given as a stream of hex character) and stored in a 'program-file.'

		// programs are loaded into ... disk ...
		// according to the format of the batched user programs in the 'program-file'

		std::ifstream inFile;
		inFile.open(filename);
		std::string currentLine;
		std::vector<std::string> tokens;
		ProgramSection currentSection = ProgramSection::NONE;
		ProcessControlBlock* currentPCB = NULL;

		if (jobQueue == NULL)
			jobQueue = new SafeQueue<ProcessControlBlock>();

		// used as starting point for writing job to disk
		Word currentJobStartAddress = 0;

		if (inFile.fail())
		{
			// ERROR
			return;
		}

		do
		{
			std::getline(inFile, currentLine);

			// Note: all the numbers are in hex

			if (currentLine.size() < 2)
			{
				// ERROR
			}
			else
			{
				tokens = tokenize(currentLine);

				if (tokens.empty())
				{
					// ERROR
					return;
				}
				else
				{
					if (boost::iequals(tokens[0], "//"))
					{
						// ** Control Card **

						// process (strip off) the special control "cards"

						if (boost::iequals(tokens[1], "JOB"))
						{
							// ** Job - Control Card **
							currentSection = ProgramSection::TEXT;

							// ( e.g. ... "// Job 1 17 2" )
							// JOB <job_id> <number_of_instruction_words> <priority_number>
							// (i.e. <processId> <codeSize> <priority>)

							// Note: following the job control card are the actual instructions

							// TODO:: Loader:: using system call (e.g. fork()) ... to create new job

							// create new job (i.e. process) & store it in a list
							if (currentPCB == NULL)
							{
								currentPCB = new ProcessControlBlock();
							}
							else
							{
								// ERROR
								// 'END' Control Card must be missing
							}

							if (tokens.size() == 5)
							{
								// store data, extracted from control cards, in PCB

								currentPCB->processId = stoi(changeBase(
									NumberSystem::HEXADECIMAL,
									NumberSystem::DECIMAL,
									ByteOrder::BIG_ENDIAN,
									std::string(tokens[2])));

								currentPCB->memoryInfo.codeSizeInWords = stoi(changeBase(
									NumberSystem::HEXADECIMAL,
									NumberSystem::DECIMAL,
									ByteOrder::BIG_ENDIAN,
									std::string(tokens[3])));

								currentPCB->schedulingInfo.priority = stoi(changeBase(
									NumberSystem::HEXADECIMAL,
									NumberSystem::DECIMAL,
									ByteOrder::BIG_ENDIAN,
									std::string(tokens[4])));
							}
							else
							{
								// ERROR
							}
						}
						else if (boost::iequals(tokens[1], "DATA"))
						{
							// ** Data - Control Card **
							currentSection = ProgramSection::DATA;

							// ( e.g. ... "// Data 14 C C" )
							// Data <input_buffer_size> <output_buffer_size> <temporary_buffer_size>

							// Note: buffer size = number of words that can be put in buffer

							if (currentPCB == NULL)
							{
								// ERROR
							}
							else
							{
								if (tokens.size() == 5)
								{
									// store data, extracted from control cards, in PCB

									currentPCB->memoryInfo.inputBufferSize = stoi(changeBase(
										NumberSystem::HEXADECIMAL,
										NumberSystem::DECIMAL,
										ByteOrder::BIG_ENDIAN,
										std::string(tokens[2])));

									currentPCB->memoryInfo.outputBufferSize = stoi(changeBase(
										NumberSystem::HEXADECIMAL,
										NumberSystem::DECIMAL,
										ByteOrder::BIG_ENDIAN,
										std::string(tokens[3])));

									currentPCB->memoryInfo.temporaryBufferSize = stoi(changeBase(
										NumberSystem::HEXADECIMAL,
										NumberSystem::DECIMAL,
										ByteOrder::BIG_ENDIAN,
										std::string(tokens[4])));
								}
								else
								{
									// ERROR
								}
							}
						}
						else if (boost::iequals(tokens[1], "END"))
						{
							// ** End - Control Card **
							currentSection = ProgramSection::NONE;

							// ( e.g. ... "// END" )

							if (currentPCB == NULL)
							{
								// ERROR
							}
							else
							{
								if (currentPCB->memoryInfo.codeSizeInWords == 0 || currentPCB->memoryInfo.dataSizeInWords == 0)
								{
									// ERROR
								}
								else
								{
									// IMPORTANT:: must set base and limit registers before inserting process into memory
									currentPCB->memoryInfo.baseRegister = currentJobStartAddress;
									currentPCB->memoryInfo.limitRegister = (currentPCB->memoryInfo.codeSizeInWords + currentPCB->memoryInfo.dataSizeInWords) - 1;

									currentPCB->memoryInfo.inputBufferStartAddress = currentPCB->memoryInfo.codeSizeInWords * WORD_SIZE_IN_BYTES;
									currentPCB->memoryInfo.outputBufferStartAddress = currentPCB->memoryInfo.inputBufferStartAddress + currentPCB->memoryInfo.inputBufferSize * WORD_SIZE_IN_BYTES;
									currentPCB->memoryInfo.temporaryBufferStartAddress = currentPCB->memoryInfo.outputBufferStartAddress + currentPCB->memoryInfo.outputBufferSize * WORD_SIZE_IN_BYTES;

									// fill buffers using data section
									if (mmu->MEMORY_MANAGEMENT_POLICY == MemoryManagementPolicy::PAGING)
									{

									}
									else
									{
										int currentPosition = 0;
										int dataSize = currentPCB->memoryInfo.data.size();

										for (size_t i = 0; i < currentPCB->memoryInfo.inputBufferSize && currentPosition < dataSize; i++)
										{
											currentPCB->context.inputBuffer.push_back(
												currentPCB->memoryInfo.data[currentPosition++]
											);
										}

										for (size_t i = 0; i < currentPCB->memoryInfo.outputBufferSize && currentPosition < dataSize; i++)
										{
											currentPCB->context.outputBuffer.push_back(
												currentPCB->memoryInfo.data[currentPosition++]
											);
										}

										for (size_t i = 0; i < currentPCB->memoryInfo.temporaryBufferSize && currentPosition < dataSize; i++)
										{
											currentPCB->context.temporaryBuffer.push_back(
												currentPCB->memoryInfo.data[currentPosition++]
											);
										}
									}

									// load current job into memory (i.e. job pool)
									currentPCB->schedulingInfo.setLoadTimeNow();
									bool processInserted = mmu->insertProcess(MemoryType::DISK, *currentPCB);

									if (processInserted)
									{
										// reposition start address for next job
										currentJobStartAddress += currentPCB->memoryInfo.limitRegister;

										jobQueue->push(*currentPCB);
										numLoadedJobs++;
									}
									else
									{
										// TODO:: Loader:: failed to insert process
										// try defrag
										//mmu->defragDisk();
										// try insert again
									}

									// parsed program successfully, stop pointing to PCB
									currentPCB = NULL;
								}
							}
						}
						else
						{
							// ERROR
						}
					}
					else if (boost::iequals(currentLine.substr(0, 2), "0X"))
					{
						// ** hex-code instructions **

						if (currentPCB == NULL)
						{
							// ERROR
						}
						else
						{
							if (currentLine.size() > 2)
							{
								// insert hex-code or instructions into RAM
								Word instruction;
								instruction.fromHexString(currentLine.substr(2));

								switch (currentSection)
								{
								case ProgramSection::NONE:
									// ERROR
									break;

								case ProgramSection::TEXT:
									// following the job control card are the actual instructions
									currentPCB->memoryInfo.text.push_back(instruction);
									break;

								case ProgramSection::DATA:
									currentPCB->memoryInfo.data.push_back(instruction);
									currentPCB->memoryInfo.dataSizeInWords++;
									break;

								default:
									// ERROR
									break;
								}
							}
							else
							{
								// ERROR
							}
						}
					}
					else
					{
						// ERROR
					}
				}
			}

		} while (!inFile.eof());

		inFile.close();
	}

	int getNumLoadedJobs()
	{
		return numLoadedJobs;
	}

private:
	MemoryManagementUnit *mmu;
	SafeQueue<ProcessControlBlock> *jobQueue;
	int numLoadedJobs;
};
