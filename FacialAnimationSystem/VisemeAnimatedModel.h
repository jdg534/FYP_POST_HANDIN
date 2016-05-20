#ifndef _VISEME_ANIMATED_MODEL_H_
#define _VISEME_ANIMATED_MODEL_H_

#include <vector>

#include <d3d11.h>

#include "Viseme.h"
#include "DialoguePattern.h"
#include "VisemeMap.h"
#include "PhonemeToVisemeMap.h"

#include "ErrorLog.h"

#include "Math.h"

struct AnimatedMesh
{
	ID3D11Buffer * vertBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexBufferOffSet;
	UINT vertexBufferStrideSize;

	unsigned int nIndercies;

	Material material;
};

class VisemeAnimatedModel
{
public:
	

	bool init(ID3D11Device * d3d11DevicePtr, DialoguePattern * initDP, VisemeMap * vm, PhonemeToVisemeMap * ptvm, bool loopedAnimation = true);
	void update(float dt, ID3D11DeviceContext * d3dDC);

	std::vector<AnimatedMesh> getAnimatedMeshes()
	{
		return m_activeAnimationState;
	}

	DirectX::XMFLOAT4X4 getWorldMatrix()
	{
		return m_worldMatrix;
	}

	void setWorldMatrix(DirectX::XMFLOAT4X4 m) { m_worldMatrix = m; }

	void shutdown();

private:

	float calculateLerpValue(float min, float max, float x);

	DirectX::XMFLOAT4X4 m_worldMatrix;

	bool m_isAnimationLooped;

	// std::vector<VisemeData> m_visemes;

	std::vector<VisemeState> m_animationStates;

	std::vector<AnimatedMesh> m_activeAnimationState;

	float m_secondsIntoAnimation;

	float m_timeToTransitionToNextState; // cuts into the time allocated fro the current state

	std::vector<std::vector<Vertex>> m_vertexData;
};

#endif