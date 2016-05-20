#include "PhonemeMap.h"
#include "ErrorLog.h"

#include <fstream>

PhonemeMap::PhonemeMap()
{

}

PhonemeMap::~PhonemeMap()
{

}

bool PhonemeMap::init(std::string phonemeFolder, std::string indexfileName)
{
	m_phonemeFolder = phonemeFolder;
	m_indexFile = indexfileName;

	std::string actualFileStr = phonemeFolder + indexfileName;

	std::ifstream file(actualFileStr);

	if (!file.good())
	{
		ErrorLog * el = ErrorLog::getInstance();
		el->logMinorError("Couldn't open: " + actualFileStr);
		return false;
	}

	std::string currentLine = "startRead";
	while (currentLine != "END")
	{
		std::getline(file, currentLine);

		// following structure: "symbol" : "file"
		if (currentLine != "END")
		{
			std::vector<int> quoteIndex;
			for (int i = 0; i < currentLine.size(); i++)
			{
				if (currentLine[i] == '"')
				{
					quoteIndex.push_back(i);
				}
			}

			// seperate the symbol string & the fileString
			std::string symbolStr;
			symbolStr = currentLine.substr(quoteIndex[0] + 1, quoteIndex[1] - (quoteIndex[0] + 1));

			std::string phonemeFileStr = currentLine.substr(quoteIndex[2] + 1, quoteIndex[3] - (quoteIndex[2] + 1));

			std::string pathToFile = m_phonemeFolder + phonemeFileStr;

			// would need to alter the / pending on the platform

			Phoneme	* tp = new Phoneme();
			tp->symbol = symbolStr;
			tp->soundDataPtr = new SoundData();
			
			if (!tp->soundDataPtr->loadFromFile(pathToFile))
			{
				// couldn't load the phoneme, this is a fatal error
				delete tp;
				ErrorLog * el = ErrorLog::getInstance();
				el->logMinorError("Couldn't load a phoneme file (see next line)");
				el->logFatalError(pathToFile);
			}
			else
			{
				m_phonemes.push_back(tp);
			}
		}
	}

	return true;
}

void PhonemeMap::shutdown()
{
	for (int i = 0; i < m_phonemes.size(); i++)
	{
		delete m_phonemes[i];
	}
	m_phonemes.clear();
}

std::vector<std::string> PhonemeMap::getPhonemeSymbols()
{
	std::vector<std::string> rv;
	for (int i = 0; i < m_phonemes.size(); i++)
	{
		rv.push_back(m_phonemes[i]->symbol);
	}
	return rv;
}

Phoneme * PhonemeMap::getPhonemeWithSymbol(std::string symbol)
{
	for (int i = 0; i < m_phonemes.size(); i++)
	{
		if (m_phonemes[i]->symbol == symbol)
		{
			return m_phonemes[i];
		}
	}
	return nullptr;
}