#ifndef _VISEME_LOADER_H_
#define _VISEME_LOADED_H_

// rember Visemes are only going to be loaded via .obj models

#include <vector>
#include <string>

#include "Viseme.h"

class VisemeLoader
{
public:
	static VisemeLoader * getInstance();

	~VisemeLoader(); // only needed as part of the Singlaton, this class doesn't actually store any data

	VisemeData * loadViseme(std::string folderPath, std::string fileStr, std::string visemeSymbol);

private:
	VisemeLoader(); // Singlaton (Needs to be private)

	void getIndicesFromObjStr(std::string objStr, int * out);

};


#endif