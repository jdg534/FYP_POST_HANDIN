#ifndef _VISEME_H_
#define _VISEME_H_

#include "RenderingStructs.h"

#include <vector>
#include <string>

#include "MathTypes.h" // part of Josh maths

#include <d3d11.h>

struct MeshData // stores: a material, vetices & normal. this is for lerping the between the states
{
	std::vector<Vertex> vertices;

	Material mat;

	bool canLerpWith(const MeshData & md)
	{
		if (vertices.size() != md.vertices.size())
		{
			return false;
		}
		/*
		else if (normals.size() != md.normals.size())
		{
			return false;
		}
		*/
		/* (might be unessary)
		if (!(mat == md.mat))
		{
			return false;
		}
		*/

		return true;
	}
};

struct VisemeData
{
	std::vector<MeshData> meshes;
	
	std::string symbol;

	bool canLerpWith(const VisemeData & vd)
	{
		if (meshes.size() != vd.meshes.size())
		{
			return false;
		}

		for (int i = 0; i < meshes.size(); i++)
		{
			if (!meshes[i].canLerpWith(vd.meshes[i]))
			{
				return false;
			}
		}
		return true;
	}

	bool canLerpWith(const VisemeData * vd)
	{
		if (meshes.size() != vd->meshes.size())
		{
			return false;
		}
		for (auto i = 0; i < meshes.size(); i++)
		{
			if (!meshes[i].canLerpWith(vd->meshes[i]))
			{
				return false;
			}
		}
	}
};

struct VisemeState
{
	float startTime, endTime;
	VisemeData data;

	bool operator<(const VisemeState & vs)
	{
		return startTime < vs.startTime;
	}
};


#endif