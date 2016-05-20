#include "PhonemeToVisemeMap.h"

#include <fstream>

PhonemeToVisemeMap * f_ptvm = nullptr;

PhonemeToVisemeMap * PhonemeToVisemeMap::getInstance()
{
	if (f_ptvm == nullptr)
	{
		f_ptvm = new PhonemeToVisemeMap();
	}
	return f_ptvm;
}

PhonemeToVisemeMap::~PhonemeToVisemeMap()
{
	f_ptvm = nullptr;
}

bool PhonemeToVisemeMap::initFromFile(std::string mapFilePathAndName)
{
	// JSON like structure per line
	// "<phoneme symbol>" : "<viseme symbol>"
	std::ifstream f(mapFilePathAndName);

	if (!f.good())
	{
		return false;
	}

	std::string linestr = "";

	while (!f.eof())
	{
		std::getline(f, linestr);
		// "<phoneme symbol>" : "<viseme symbol>"
		if (linestr == "END")
		{
			// do nothing, it's the end of the file anyway
		}
		else
		{
			int midPoint;
			int q1, q2;

			midPoint = linestr.find_last_of(":");

			std::string phonemeSymbol, visemeSymbol;
			phonemeSymbol = linestr.substr(0, midPoint - 1);

			visemeSymbol = linestr.substr(midPoint + 1, linestr.size() - 1);

			// loose the quotes
			q1 = phonemeSymbol.find_first_of("\"");
			q2 = phonemeSymbol.find_last_of("\"");

			phonemeSymbol = phonemeSymbol.substr(q1 + 1, q2 - 1);
			
			// now the viseme symbol
			q1 = visemeSymbol.find_first_of("\"");
			q2 = visemeSymbol.find_last_of("\"");

			visemeSymbol = visemeSymbol.substr(q1 + 1, q2 - q1 - 1);

			PhonemeToVisemeConnection tempConnection;
			tempConnection.phonemeSymbol = phonemeSymbol;
			tempConnection.visemeSymbol = visemeSymbol;

			m_connections.push_back(tempConnection);
		}
	}
}

std::string PhonemeToVisemeMap::getCorrispondingVisemeSymbol(std::string phonemeSymbol)
{
	for (auto i = 0; i < m_connections.size(); i++)
	{
		if (phonemeSymbol == m_connections[i].phonemeSymbol)
		{
			return m_connections[i].visemeSymbol;
		}
	}
	return  m_connections[m_connections.size() - 1].visemeSymbol; // default silence last
}

void PhonemeToVisemeMap::shutdown()
{
	m_connections.clear();
}

PhonemeToVisemeMap::PhonemeToVisemeMap()
{

}