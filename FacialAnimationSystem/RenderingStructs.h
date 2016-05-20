#ifndef _RENDERING_STRUCTS_H_
#define _RENDERING_STRUCTS_H_

#include <DirectXMath.h> // moving the rendering to use D3D11

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};

struct Light
{
	DirectX::XMFLOAT4 AmbientLightComponent;
	DirectX::XMFLOAT4 DiffuseLightComponent;
	DirectX::XMFLOAT4 SpecularLightComponent;

	float SpecularPower;
	DirectX::XMFLOAT3 LightVecW;
	DirectX::XMFLOAT3 lightPosW;


	void operator =(const Light & l)
	{
		AmbientLightComponent = l.AmbientLightComponent;
		DiffuseLightComponent = l.DiffuseLightComponent;
		SpecularLightComponent = l.SpecularLightComponent;
		SpecularPower = l.SpecularPower;
		LightVecW = l.LightVecW;
		lightPosW = l.lightPosW;
	}
};

struct Material
{
	DirectX::XMFLOAT4 AmbientComponent;
	DirectX::XMFLOAT4 DiffuseComponent;
	DirectX::XMFLOAT4 SpecularComponent;

	void operator = (const Material & a)
	{
		AmbientComponent = a.AmbientComponent;
		DiffuseComponent = a.DiffuseComponent;
		SpecularComponent = a.SpecularComponent;
	}

	bool operator ==(const Material & a)
	{
		if (AmbientComponent.x != a.AmbientComponent.x)
			return false;
		else if (AmbientComponent.y != a.AmbientComponent.y)
			return false;
		else if (AmbientComponent.z != a.AmbientComponent.z)
			return false;
		else if (AmbientComponent.w != a.AmbientComponent.w)
			return false;
		
		else if (DiffuseComponent.x != a.DiffuseComponent.x)
			return false;
		else if (DiffuseComponent.y != a.DiffuseComponent.y)
			return false;
		else if (DiffuseComponent.z != a.DiffuseComponent.z)
			return false;
		else if (DiffuseComponent.w != a.DiffuseComponent.w)
			return false;

		else if (SpecularComponent.x != a.SpecularComponent.x)
			return false;
		else if (SpecularComponent.y != a.SpecularComponent.y)
			return false;
		else if (SpecularComponent.z != a.SpecularComponent.z)
			return false;
		else if (SpecularComponent.w != a.SpecularComponent.w)
			return false;

		return true;
	}

	Material()
	{
		AmbientComponent.x = 0.2f;
		AmbientComponent.y = 0.2f;
		AmbientComponent.z = 0.2f;
		AmbientComponent.w = 1.0f;
		
		DiffuseComponent.x = 0.6f;
		DiffuseComponent.y = 0.6f;
		DiffuseComponent.z = 0.6f;
		DiffuseComponent.w = 1.0f;

		SpecularComponent.x = 0.2f;
		SpecularComponent.y = 0.2f;
		SpecularComponent.z = 0.2f;
		SpecularComponent.w = 1.0f;
	}
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	Material mMaterial;
	Light mLight;

	DirectX::XMFLOAT3 eyePositionWorldSpace;
	
};

#endif