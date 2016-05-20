#include "VisemeLoader.h"

#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "ErrorLog.h"
#include "MaterialManager.h"



VisemeLoader * f_VLPtr = nullptr; // Filescope_VisemeLoaderPointer

VisemeLoader * VisemeLoader::getInstance()
{
	if (f_VLPtr == nullptr)
	{
		f_VLPtr = new VisemeLoader();
	}
	return f_VLPtr;
}

VisemeLoader::~VisemeLoader()
{
	f_VLPtr = nullptr;
}

VisemeData * VisemeLoader::loadViseme(std::string folderPath, std::string fileStr, std::string visemeSymbol)
{
	// rember texture coords don't matter, is multiple Meshes
	std::string actualFileString = folderPath + "/" + fileStr;

	std::ifstream modelFile(actualFileString);

	ErrorLog * el = ErrorLog::getInstance();

	if (!modelFile.good())
	{
		el->logMinorError("Model file not found: " + actualFileString);
		return nullptr;
	}

	VisemeData * rv = new VisemeData();
	rv->symbol = visemeSymbol;

	std::string currentBit = "";
	std::string previousFlag = "";

	MeshData mdTmp;
	
	std::vector<DirectX::XMFLOAT3> verticesForMesh;
	std::vector<DirectX::XMFLOAT3> normalsForMesh;
	std::vector<Vertex> vertexDataToStore;

	MaterialManager * mmPtr = MaterialManager::getInstance();

	NamedMaterial * nm = nullptr;

	while (!modelFile.eof())
	{
		modelFile >> currentBit;
		bool handledARecognisedFlag = false;

		if (currentBit == "mtllib")
		{
			std::string mtrlLibFile;
			// std::getline(modelFile, mtrlLibFile);
			modelFile >> mtrlLibFile;

			mmPtr->addMaterialsFrom(folderPath + "/"+ mtrlLibFile);
			handledARecognisedFlag = true;
		}
		else if (currentBit == "usemtl")
		{
			std::string matName;
			modelFile >> matName;

			nm = mmPtr->getMaterialWithID(matName);

			handledARecognisedFlag = true;
		}
		else if (currentBit == "v")
		{
			// check if was dealing with a face "f" last bit
			if (previousFlag == "f")
			{
				// set the mesh data to have the mesh data parsed so far
				mdTmp.mat = nm->data;
				mdTmp.vertices = vertexDataToStore;
				rv->meshes.push_back(mdTmp);

				// once set the mesh data, clear the vectors that data is read to


				// test, don't delete the values, (from looking at the index values in the test model)
				// verticesForMesh.clear();
				// normalsForMesh.clear();
				// vertexDataToStore.clear();
				vertexDataToStore.clear();
			}
			DirectX::XMFLOAT3 vTmp;
			modelFile >> vTmp.x;
			modelFile >> vTmp.y;
			modelFile >> vTmp.z;
			verticesForMesh.push_back(vTmp);
			handledARecognisedFlag = true;
		}
		else if (currentBit == "vn")
		{
			DirectX::XMFLOAT3 vnTmp;
			modelFile >> vnTmp.x;
			modelFile >> vnTmp.y;
			modelFile >> vnTmp.z;

			normalsForMesh.push_back(vnTmp);
			handledARecognisedFlag = true;
		}
		// would cover vt, if were using texture mapping
		else if (currentBit == "f")
		{
			// rember just dealing with vertex data, not index data
			Vertex vertTmp1;
			Vertex vertTmp2;
			Vertex vertTmp3;

			int indercies[3];
			indercies[0] = indercies[1] = indercies[2] = 1;
			std::string vt1s, vt2s, vt3s;

			modelFile >> vt1s >> vt2s >> vt3s;

			if (vt1s == ""
				|| vt2s == ""
				|| vt3s == "")
			{
				// the line isn't reading correctly
				std::string theFile = folderPath + "/" + fileStr;
				int pos = modelFile.tellg();
				std::stringstream ss;
				ss << "Issue loading from " << theFile << " at position " << pos;
				el->logMinorError(ss.str());
				
			}

			// v/vt/vn
			getIndicesFromObjStr(vt1s, indercies);
			vertTmp1.pos = verticesForMesh[indercies[0]-1];
			vertTmp1.normal = normalsForMesh[indercies[2]-1];

			getIndicesFromObjStr(vt2s, indercies);
			vertTmp2.pos = verticesForMesh[indercies[0] - 1];
			vertTmp2.normal = normalsForMesh[indercies[2] - 1];

			getIndicesFromObjStr(vt3s, indercies);
			vertTmp3.pos = verticesForMesh[indercies[0] - 1];
			vertTmp3.normal = normalsForMesh[indercies[2] - 1];

			vertexDataToStore.push_back(vertTmp1);
			vertexDataToStore.push_back(vertTmp2);
			vertexDataToStore.push_back(vertTmp3);

			handledARecognisedFlag = true;
		}

		// after checking for the flags, set the previous flag value
		if (handledARecognisedFlag)
		{
			previousFlag = currentBit;
		}
		
	}

	mdTmp.mat = nm->data;
	mdTmp.vertices = vertexDataToStore;
	rv->meshes.push_back(mdTmp);

	return rv;
}


VisemeLoader::VisemeLoader()
{
	// nothing to init
}

void VisemeLoader::getIndicesFromObjStr(std::string objStr, int * out)
{
	int nSlashes = 0;
	for (auto i = 0; i < objStr.size(); i++)
	{
		if (objStr[i] == '/')
		{
			nSlashes++;
		}
	}
	if (nSlashes < 2)
	{
		 out[0] = out[1] = out[2] = 1;
		 return;
	}


	// asumes "i/i/i"
	int s1, s2;
	s1 = objStr.find_first_of("/");
	s2 = objStr.find_last_of("/");
	
	std::string ss1, ss2, ss3;
	ss1 = objStr.substr(0, s1);
	ss2 = objStr.substr(s1 + 1, s2 - s1 - 1);
	ss3 = objStr.substr(s2 + 1, objStr.size() - s2);

	out[0] = std::atoi(ss1.c_str());
	out[1] = std::atoi(ss2.c_str());
	out[2] = std::atoi(ss3.c_str());
}