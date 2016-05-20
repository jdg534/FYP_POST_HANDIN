#ifndef _PHONEME_TO_VISEME_MAP_H_
#define _PHONEME_TO_VISEME_MAP_H_

#include <vector>
#include <string>

struct PhonemeToVisemeConnection
{
	std::string phonemeSymbol, visemeSymbol;
};

class PhonemeToVisemeMap
{
public:
	static PhonemeToVisemeMap * getInstance();
	~PhonemeToVisemeMap();
	bool initFromFile(std::string mapFilePathAndName);


	std::string getCorrispondingVisemeSymbol(std::string phonemeSymbol);

	void shutdown();

private:
	PhonemeToVisemeMap(); // Singlaton, must be private
	std::vector<PhonemeToVisemeConnection> m_connections;
};



#endif