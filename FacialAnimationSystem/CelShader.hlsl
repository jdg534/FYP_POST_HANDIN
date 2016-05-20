struct ShaderMaterialStruct
{
	float4 AmbMtrlComponent;
	float4 diffMtrlComponent;
	float4 specMtrlComponent;
};

struct ShaderLightStruct
{
	float4 ambLightComponent;
	float4 diffLightComponent;
	float4 specLightComponent;

	float specularPower;
	float3 lightVecWorldSpace;
	float3 lightPosWorldSpace;
};

// now the constant buffer

cbuffer ConstantBuffer : register (b0)
{
	matrix cb_worldMat;
	matrix cb_viewMat;
	matrix cb_projectionMat;

	ShaderMaterialStruct cb_material;
	ShaderLightStruct cb_light;

	float3 cb_EyePosWorldSpace;
}

struct VS_INPUT_STRUCT
{
	float4 PosLocalSpace : POSITION;
	float3 NormalLocalSpace : NORMAL;
};

struct VS_OUTPUT_STRUCT // this is what's returned by the vertex Shader
{
	float4 finalPosition : SV_POSITION;
	float3 NormalWorldSpace : NORMAL;

	float3 PosWorldSpace : POSITION;
};

VS_OUTPUT_STRUCT VertexShaderMain(VS_INPUT_STRUCT vsIn)
{
	VS_OUTPUT_STRUCT rv = (VS_OUTPUT_STRUCT)0;

	float4 PosWorldSpace = mul(vsIn.PosLocalSpace, cb_worldMat);
	rv.PosWorldSpace = PosWorldSpace.xyz;

	rv.finalPosition = mul(PosWorldSpace, cb_viewMat);
	rv.finalPosition = mul(rv.finalPosition, cb_projectionMat);

	float3 normalWorldSpace = mul(float4(vsIn.NormalLocalSpace, 0.0f), cb_worldMat);
	rv.NormalWorldSpace = normalize(normalWorldSpace);
	return rv; // passes rv to the pixel shader
}

float4 pixelShaderMain(VS_OUTPUT_STRUCT psIn) : SV_Target
{
	psIn.NormalWorldSpace = normalize(psIn.NormalWorldSpace);
	// using information from the OpenGL 4.0 Shading language cookbook
	
	float3 lightToPixelPos = psIn.PosWorldSpace - cb_light.lightPosWorldSpace;
	lightToPixelPos = normalize(lightToPixelPos);

	float cosNormalWithLight = max(0.0f, dot(lightToPixelPos, psIn.NormalWorldSpace));

	int nColourLevels = 3;
	float scaleFactor = 1.0f / nColourLevels;

	float colourScaler = 0.0f;
	
	if (cosNormalWithLight >= 0.0f && cosNormalWithLight < 0.25f)
	{
		colourScaler = 0.0f;
	}
	else if (cosNormalWithLight >= 0.25f && cosNormalWithLight < 0.50f)
	{
		colourScaler = 0.25f;
	}
	else if (cosNormalWithLight >= 0.50f && cosNormalWithLight < 0.75f)
	{
		colourScaler = 0.50f;
	}
	else if (cosNormalWithLight >= 0.75f && cosNormalWithLight < 1.0f)
	{
		colourScaler = 0.75f;
	}


	float4 diffColour = cb_light.diffLightComponent * cb_material.diffMtrlComponent;
		// only dealing with diffuse light, for cel shading

	float4 finalColour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	finalColour = diffColour * colourScaler;

	// add amient, for now
	float3 ambColour = (cb_light.ambLightComponent * cb_material.AmbMtrlComponent).xyz;
	finalColour.xyz += ambColour;

	return finalColour;
}