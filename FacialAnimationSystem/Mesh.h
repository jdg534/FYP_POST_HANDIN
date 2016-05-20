#ifndef _MESH_H_
#define _MESH_H_

#include "MathTypes.h"
#include <vector>

#include <DirectXMath.h>
#include <d3d11.h>

#include "RenderingStructs.h"

struct Triangle
{
	unsigned int v0, v1, v2;
};

class Mesh
{
	// this Class just deals with geomatry

public:
	Mesh();
	~Mesh();


	void draw(ID3D11DeviceContext * d3dDeviceContext);


	bool initTestCube(ID3D11Device * d3d11DevicePtr);

	void shutdown();

	Material getMaterial(){ return m_material; }
	DirectX::XMMATRIX getWorldMatrix(){ return DirectX::XMLoadFloat4x4(&m_worldMatrix); }

private:


	std::vector<Vertex> m_vertices;
	
	Material m_material;

	
	Material m_objectMaterial;

	DirectX::XMFLOAT4X4 m_worldMatrix;

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;

	unsigned int m_nIndercies;
	UINT m_vertexStride;
	UINT m_vertexBufferOffset;
};


#endif