#ifndef _PHONEME_MAP_H_
#define _PHONEME_MAP_H_

#include <string>
#include <vector>
#include "SoundData.h"

struct Phoneme
{
	std::string symbol;
	SoundData * soundDataPtr;
};

class PhonemeMap
{
public:
	PhonemeMap();
	~PhonemeMap();

	bool init(std::string phonemeFolder, std::string indexfileName);
	void shutdown();

	std::vector<std::string> getPhonemeSymbols();

	Phoneme * getPhonemeWithSymbol(std::string symbol);

private:
	std::string m_phonemeFolder;
	std::string m_indexFile;
	std::vector<Phoneme *> m_phonemes;
};


#endif