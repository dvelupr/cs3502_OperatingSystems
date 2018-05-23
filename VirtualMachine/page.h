#pragma once

#include "word.h"
#include <string>
#include <vector>

#ifndef PAGE_SIZE_IN_WORDS
#define PAGE_SIZE_IN_WORDS 4
#endif // PAGE_SIZE_IN_WORDS

class Page
{
public:
	Page(int numWordsInPage = PAGE_SIZE_IN_WORDS)
	{
		if (numWordsInPage <= 0)
		{
			// ERROR
			// must have at least 1 word in a page
			words = new std::vector<Word>(1);
		}
		else
		{
			words = new std::vector<Word>(numWordsInPage);
		}
		
		free = true;
	}

	~Page()
	{
		delete words;
	}

	Word read(int offset) const
	{
		Word nullWord;
		nullWord.setLabel("NULL");

		if (offset < words->size())
			return words->at(offset);
		else
			return nullWord;
	}

	// @return bool, true == success
	bool write(int offset, Word newWord)
	{
		if (offset < words->size())
		{
			words->at(offset) = newWord;
			free = false;
			return true;
		}
		else
		{
			return false; // ERROR
		}
	}

	void zeroFill()
	{
		if (words == NULL)
			return; // ERROR

		for (size_t i = 0; i < words->size(); i++)
			words->at(i) = Word(0);
	}

	void makeFree(bool fillWithZeros = true)
	{
		if(fillWithZeros)
			zeroFill();

		free = true;
	}

	int getSizeInBytes()
	{
		return (words->size() * WORD_SIZE_IN_BYTES);
	}

	int getSizeInWords()
	{
		return words->size();
	}

	// TODO:: Page:: set free

	bool isFree()
	{
		return free;
	}

	std::string toString(
		std::string indent = "",
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		bool usePrefix = true,
		bool useLeadingZeros = true) const
	{
		std::string retval = "";

		if (words == NULL)
		{
			retval.append(indent);
			retval.append("NULL");
		}
		else
		{
			switch (numberSystem)
			{
			case NumberSystem::BINARY:
				for (size_t i = 0; i < words->size(); i++)
				{
					retval.append(indent);
					retval.append("word[" + std::to_string(i) + "]:\t\t");
					retval.append(words->at(i).toBinaryString(false));
					retval.append("\n");
				}
				break;

			case NumberSystem::DECIMAL:
				for (size_t i = 0; i < words->size(); i++)
				{
					retval.append(indent);
					retval.append("word[" + std::to_string(i) + "]:\t\t");
					retval.append(words->at(i).toString());
					retval.append("\n");
				}
				break;

			case NumberSystem::HEXADECIMAL:
				for (size_t i = 0; i < words->size(); i++)
				{
					retval.append(indent);
					retval.append("word[" + std::to_string(i) + "]:\t\t");
					retval.append(words->at(i).toHexString(true, true));
					retval.append("\n");
				}
				break;

			default:
				// ERROR
				break;
			}
		}
		retval.append("\n");

		return retval;
	}

	static std::string vectorToString(
		const std::vector<Page> *pageVector,
		std::string indent = "",
		NumberSystem numberSystem = NumberSystem::HEXADECIMAL,
		bool usePrefix = true,
		bool useLeadingZeros = true)
	{
		std::string retval;

		retval.append("\n");
		retval.append(indent);
		retval.append("----------------------------------------");
		retval.append("\n");

		if (pageVector != NULL)
		{
			for (int i = 0; i < pageVector->size(); i++)
			{
				retval.append(indent);
				retval.append("Page #" + std::to_string(i));
				retval.append("\n");

				retval.append(indent);
				retval.append("________________________________________");
				retval.append("\n");

				retval.append(pageVector->at(i).toString(indent, numberSystem, usePrefix, useLeadingZeros));
				retval.append("\n");
			}
		}
		else
		{
			retval.append(indent);
			retval.append("NULL");
			retval.append("\n");
		}

		retval.append(indent);
		retval.append("-----------------------------------------");
		retval.append("\n");

		return retval;
	}

private:
	std::vector<Word> *words;
	bool free;
};
