#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_


#include "RenderingStructs.h"

#include <d3d11.h>

#include <vector>


// this is a modified class that is based on a class i submitted as part of an assignment for 3DCG

struct NamedMaterial
{
	std::string Name; // NO SHAREING!!!
	Material data;

	NamedMaterial()
	{
		Name = "UNAMED_MATERIAL";
		data = Material(); // default setup
	}
};

class MaterialManager
{
public:
	static MaterialManager * getInstance();

	~MaterialManager();

	void addMaterialsFrom(std::string materialFileName);
	NamedMaterial * getMaterialWithID(std::string materialID);

	bool addMaterial(NamedMaterial *m);

	void shutdown();

	bool isNameAvailable(std::string matNameToCheck);

private:
	MaterialManager(); // singleton

	std::vector<NamedMaterial *> m_materials;
	
	
};


#endif