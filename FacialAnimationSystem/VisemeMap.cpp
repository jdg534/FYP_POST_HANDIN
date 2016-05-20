#include "VisemeMap.h"
#include "VisemeLoader.h"

#include "ErrorLog.h"

#include <fstream>

bool VisemeMap::initFromFile(std::string visemeMapFile)
{
	/*	the structure:
	visemeFolder: "TestVisemes"
	"<symbol here>" : "<model file name here>"
	...
	END
	*/
	std::string visemeFolder = "", s = "";
	std::ifstream file(visemeMapFile);

	ErrorLog * el = ErrorLog::getInstance();

	if (!file.good())
	{
		el->logFatalError("Couldn't load: " + visemeMapFile);
		return false;
	}
	file >> s; // visemeFolder:
	file >> visemeFolder;

	visemeFolder = visemeFolder.substr(1, visemeFolder.size() - 2);

	VisemeLoader * vl;

	vl = VisemeLoader::getInstance();

	std::getline(file, s);

	while (s != "END")
	{
		std::getline(file, s);
		if (s != "END")
		{
			// "<symbol here>" : "<model file name here>"

			

			int splitPoint = s.find_first_of(":");
			std::string symbol = "", modelFile = "";
			int q1, q2;
			symbol = s.substr(0, splitPoint);
			q1 = symbol.find_first_of("\"");
			q2 = symbol.find_last_of("\"");
			symbol = symbol.substr(q1 + 1, q2 - 1);

			// now the model file
			modelFile = s.substr(splitPoint, s.size());
			q1 = modelFile.find_first_of("\"");
			q2 = modelFile.find_last_of("\"");
			modelFile = modelFile.substr(q1 + 1, (q2 - q1) - 1);

			// std::string filePath = visemeFolder + "/" + modelFile;
			VisemeData * vdTmp = vl->loadViseme(visemeFolder, modelFile, symbol);

			if (vdTmp == nullptr)
			{
				// failed to load
				delete vdTmp;
			}
			else
			{
				// loaded from file
				m_visemes.push_back(vdTmp);
			}
		}
	}

	// now check all of the visemes can lerp between the other visemes
	for (auto i = 0; i < m_visemes.size(); i++)
	{
		for (auto j = 0; j < m_visemes.size(); j++)
		{
			if (i == j)
			{
				// skip the comparison, obviously a viseme will be compaterble with itself
			}
			else if (m_visemes[i]->canLerpWith(m_visemes[j]))
			{
				// the visemes are compatable
			}
			else
			{
				// not compaterble
				shutdown();
				return false;
			}
		}
	}

	return true;
}

std::vector<std::string> VisemeMap::getVisemeSymbols()
{
	std::vector<std::string> rv;
	for (int i = 0; i < m_visemes.size(); i++)
	{
		rv.push_back(m_visemes[i]->symbol);
	}
	return rv;
}

VisemeData * VisemeMap::getVisemeWithMatchingSymol(std::string visemeSymbol)
{
	for (int i = 0; i < m_visemes.size(); i++)
	{
		if (m_visemes[i]->symbol == visemeSymbol)
		{
			return m_visemes[i];
		}
	}
	return nullptr;
}

void VisemeMap::shutdown()
{
	for (int i = 0; i < m_visemes.size(); i++)
	{
		for (int j = 0; j < m_visemes[i]->meshes.size(); j++)
		{
			m_visemes[i]->meshes[j].vertices.clear();
		}
		delete m_visemes[i];
	}
	m_visemes.clear();
}