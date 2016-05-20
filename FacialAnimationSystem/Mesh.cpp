#include "Mesh.h"

#include "Math.h"




Mesh::Mesh()
{
	DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());
	m_material.AmbientComponent = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_material.DiffuseComponent = DirectX::XMFLOAT4(0.6, 0.6f, 0.6f, 1.0f);
	m_material.SpecularComponent = DirectX::XMFLOAT4(0.2, 0.2f, 0.2f, 1.0f);
}

Mesh::~Mesh()
{
	m_vertices.clear();
	
}

void Mesh::draw(ID3D11DeviceContext * d3dDeviceContext)
{
	// m_drawMode = MDM_SOLID; // it works

	/*
	if (m_drawMode == MDM_WIREFRAME)
	{
		for (int i = 0; i < m_triangles.size(); i++)
		{
			glBegin(GL_LINE_LOOP);
				glVertex3f(m_vertices[m_triangles[i].v0].x, m_vertices[m_triangles[i].v0].y, m_vertices[m_triangles[i].v0].z);
				glVertex3f(m_vertices[m_triangles[i].v1].x, m_vertices[m_triangles[i].v1].y, m_vertices[m_triangles[i].v1].z);
				glVertex3f(m_vertices[m_triangles[i].v2].x, m_vertices[m_triangles[i].v2].y, m_vertices[m_triangles[i].v2].z);
			glEnd();
		}
	}
	else
	{
		for (int i = 0; i < m_triangles.size(); i++)
		{
			glBegin(GL_TRIANGLES);
				glVertex3f(m_vertices[m_triangles[i].v0].x, m_vertices[m_triangles[i].v0].y, m_vertices[m_triangles[i].v0].z);
				glVertex3f(m_vertices[m_triangles[i].v1].x, m_vertices[m_triangles[i].v1].y, m_vertices[m_triangles[i].v1].z);
				glVertex3f(m_vertices[m_triangles[i].v2].x, m_vertices[m_triangles[i].v2].y, m_vertices[m_triangles[i].v2].z);
			glEnd();
		}
	}
	*/

	d3dDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_vertexStride, &m_vertexBufferOffset);
	d3dDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	d3dDeviceContext->DrawIndexed(m_nIndercies, 0,0);
}

bool Mesh::initTestCube(ID3D11Device * d3d11DevicePtr)
{
	Vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)},

		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f) },

		{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f) },

		{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)},

		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f)},

		{ DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)},
		{ DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f)},
	};

	int nVerts = ARRAYSIZE(vertices);

	for (int i = 0; i < nVerts; i++)
	{
		m_vertices.push_back(vertices[i]);
	}

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(vbDesc));
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Vertex) * nVerts;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbInitData;
	ZeroMemory(&vbInitData, sizeof(vbInitData));
	vbInitData.pSysMem = &m_vertices[0];
	
	d3d11DevicePtr->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer);
	
	m_vertexStride = sizeof(Vertex);
	m_vertexBufferOffset = 0;

	// now the index buffer

	// Create index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
	};

	int nIndices = ARRAYSIZE(indices);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	d3d11DevicePtr->CreateBuffer(&bd, &InitData, &m_indexBuffer);

	m_nIndercies = nIndices;

	return true;
}

void Mesh::shutdown()
{
	m_vertices.clear();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
}