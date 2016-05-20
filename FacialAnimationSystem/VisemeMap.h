#ifndef _VISEME_MAP_H_
#define _VISEME_MAP_H_

#include "Viseme.h"

class VisemeMap // like phonemeMap class
{
public:
	
	bool initFromFile(std::string visemeMapFile);

	std::vector<std::string> getVisemeSymbols();

	VisemeData * getVisemeWithMatchingSymol(std::string visemeSymbol);

	void shutdown();

private:

	std::vector<VisemeData *> m_visemes;
};


#endif