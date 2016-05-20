#include "MaterialManager.h"

#include "ErrorLog.h"

#include <fstream>


MaterialManager * fileScopePtr = nullptr;

MaterialManager * MaterialManager::getInstance()
{
	if (fileScopePtr == nullptr)
	{
		fileScopePtr = new MaterialManager();
	}
	return fileScopePtr;
}

MaterialManager::~MaterialManager()
{
	fileScopePtr = nullptr;
}

void MaterialManager::addMaterialsFrom(std::string materialFileName)
{
	std::ifstream materialFile(materialFileName);

	ErrorLog * elInst = ErrorLog::getInstance();

	if (!materialFile.good())
	{
		elInst->logFatalError("Couldn't open: " + materialFileName);
	}

	// TextureManager * tmInstance = TextureManager::getInstance();

	// No Textures!


	std::string currentBit = "";
	NamedMaterial * tmpM = nullptr;
	while (!materialFile.eof())
	{
		materialFile >> currentBit;
		if (currentBit == "newmtl")
		{
			if (tmpM != nullptr)
			{
				m_materials.push_back(tmpM);
			}
			tmpM = new NamedMaterial();
			materialFile >> tmpM->Name;

			if (!isNameAvailable(tmpM->Name))
			{
				// message box about the name conflict
				
				elInst->logMinorError("Material name conflict, " + tmpM->Name + " already defined in the material manager (this is to be expected if the models use the same material, the first difinition encountered will be used)");


				std::string skipedData = "";
				while (skipedData != "newmtl")
				{
					materialFile >> skipedData;
					// could resch the end of the file
					if (materialFile.eof())
					{
						delete tmpM;
						tmpM = nullptr;
						break;
						break;
					}
				}

				delete tmpM;
				tmpM = nullptr;


				tmpM = new NamedMaterial();
				materialFile >> tmpM->Name;
			}
		}
		else if (currentBit == "Ka")
		{
			// 3 float for ambient colour
			materialFile >> tmpM->data.AmbientComponent.x;
			materialFile >> tmpM->data.AmbientComponent.y;
			materialFile >> tmpM->data.AmbientComponent.z;
		}
		else if (currentBit == "Kd")
		{
			materialFile >> tmpM->data.DiffuseComponent.x;
			materialFile >> tmpM->data.DiffuseComponent.y;
			materialFile >> tmpM->data.DiffuseComponent.z;
		}
		else if (currentBit == "Ks")
		{
			materialFile >> tmpM->data.SpecularComponent.x;
			materialFile >> tmpM->data.SpecularComponent.y;
			materialFile >> tmpM->data.SpecularComponent.z;
		}
		else if (currentBit == "map_ka")
		{
			// no textureing

		}
		else if (currentBit == "map_kd")
		{
			// no textureing
		}
		else if (currentBit == "map_ks")
		{
			// no textureing
		}
		else if (currentBit == "map_bump" || currentBit == "bump")
		{
			// no textureing
		}
		else if (currentBit == "map_height") // not normally in .mtl spec
		{
			// no textureing
		}
	}
	if (tmpM != nullptr)
	{
		if (tmpM->Name != "UNAMED_MATERIAL")
		{
			m_materials.push_back(tmpM);
		}
	}
}

NamedMaterial * MaterialManager::getMaterialWithID(std::string materialID)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i]->Name == materialID)
		{
			return m_materials[i];
		}
	}
	return nullptr;
}

bool MaterialManager::addMaterial(NamedMaterial *m)
{
	if (!isNameAvailable(m->Name))
	{
		// adding would create name conflict
		return false;
	}
	m_materials.push_back(m);
	return true;
}

void MaterialManager::shutdown()
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		delete m_materials[i];
	}
	m_materials.clear();
}

bool MaterialManager::isNameAvailable(std::string matNameToCheck)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (matNameToCheck == m_materials[i]->Name)
		{
			return false;
		}
	}
	return true;
}

MaterialManager::MaterialManager()
{

}