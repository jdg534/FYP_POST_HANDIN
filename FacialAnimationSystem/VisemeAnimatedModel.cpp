#include "VisemeAnimatedModel.h"


bool VisemeAnimatedModel::init(ID3D11Device * d3d11DevicePtr, DialoguePattern * initDP, VisemeMap * vm, PhonemeToVisemeMap * ptvm, bool loopedAnimation)
{
	// need 2 ID3D11Buffer(s) per mesh, 1 vertex & 1 index
	//std::vector<std::string> visemeSymbols = vm->getVisemeSymbols();

	std::vector<DialogueState> dialogueStates = initDP->getDialogueStates();

	DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());

	for (auto i = 0; i < dialogueStates.size(); i++)
	{
		VisemeState vsToAdd;
		vsToAdd.startTime = dialogueStates[i].stateStart;
		vsToAdd.endTime = dialogueStates[i].stateEnd;
		// now get the viseme
		std::string visemeSymbol = ptvm->getCorrispondingVisemeSymbol(dialogueStates[i].stateStr);
		VisemeData * vdTmpPtr = vm->getVisemeWithMatchingSymol(visemeSymbol);

		if (vdTmpPtr == nullptr)
		{
			ErrorLog *el = ErrorLog::getInstance();
			el->logFatalError("Error couldn't match viseme symbol to a viseme map symbol");
		}

		vsToAdd.data.symbol = vdTmpPtr->symbol;
		vsToAdd.data.meshes = vdTmpPtr->meshes;
		m_animationStates.push_back(vsToAdd);
	}

	// now setup the vertex & index buffers
	int nMeshes = m_animationStates[0].data.meshes.size();

	m_vertexData.resize(nMeshes);

	for (auto i = 0; i < nMeshes; i++)
	{
		m_vertexData[i] = m_animationStates[0].data.meshes[i].vertices;


		AnimatedMesh amToAdd;
		int nVerts = m_animationStates[0].data.meshes[i].vertices.size();
		int nIndiercies = nVerts; // see loading code for why
		amToAdd.nIndercies = nIndiercies;
		amToAdd.vertexBufferStrideSize = sizeof(Vertex);
		amToAdd.vertexBufferOffSet = 0;

		// create the buffers

		// vertex (needs by able to be overiden)
		D3D11_BUFFER_DESC vbDesc;
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(Vertex) * nVerts;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allows the CPU to change the contents

		D3D11_SUBRESOURCE_DATA vbInitData;
		ZeroMemory(&vbInitData, sizeof(vbInitData));
		// vbInitData.pSysMem = &amToAdd.;
		vbInitData.pSysMem = &m_animationStates[0].data.meshes[i].vertices[0];

		ID3D11Buffer * vertBuffer = nullptr;


		HRESULT createBufferResults = d3d11DevicePtr->CreateBuffer(&vbDesc, &vbInitData, &vertBuffer);



		ErrorLog * el = ErrorLog::getInstance();

		if (FAILED(createBufferResults))
		{
			el->logFatalError("Couldn't create the vertex buffer for a animated mesh, result code: " + createBufferResults);
		}

		amToAdd.vertBuffer = vertBuffer;


		// now the index buffer

		std::vector<unsigned int> indercies;
		for (unsigned int i = 0; i < nIndiercies; i++)
		{
			indercies.push_back(i);
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * indercies.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &indercies[0];

		ID3D11Buffer * indexBuffer = nullptr;

		createBufferResults = d3d11DevicePtr->CreateBuffer(&bd, &InitData, &indexBuffer);



		if (FAILED(createBufferResults))
		{
			el->logFatalError("Couldn't create the index buffer for a animated mesh, result code: " + createBufferResults);
		}

		amToAdd.indexBuffer = indexBuffer;

		amToAdd.material = m_animationStates[0].data.meshes[i].mat;

		m_activeAnimationState.push_back(amToAdd);
	}

	// now validate that the visemes can be lerped between
	for (auto i = 0; i < m_animationStates.size(); i++)
	{
		bool okSoFar = true;
		for (auto j = 0; j < m_animationStates.size(); j++)
		{
			if (i == j)
			{
				// skip
			}
			else if (!m_animationStates[i].data.canLerpWith(m_animationStates[j].data))
			{
				okSoFar = false;
			}
		}
		if (!okSoFar)
		{
			return false;
		}
	}



	m_isAnimationLooped = loopedAnimation;

	m_secondsIntoAnimation = 0.0f;

	std::sort(m_animationStates.begin(), m_animationStates.end());

	m_timeToTransitionToNextState = 0.25f;// just a guess, overrire this when got a clearer picture

	return true;
}

void VisemeAnimatedModel::update(float dt, ID3D11DeviceContext * d3dDC)
{
	// determine which frames, we're lerping between, may need to bilerp or even tri lerp for transitions. (keep simple for now)
	m_secondsIntoAnimation += dt;

	if (m_secondsIntoAnimation > m_animationStates[m_animationStates.size() - 1].endTime)
	{
		// m_secondsIntoAnimation -= m_animationStates[m_animationStates.size() - 1].endTime;
		m_secondsIntoAnimation = 0.0f;
	}

	// determient the states will be lerping between

	int currentStateIndex = 0;
	for (int i = 0; i < m_animationStates.size(); i++)
	{
		if (m_secondsIntoAnimation > m_animationStates[i].startTime
			&& m_secondsIntoAnimation < m_animationStates[i].endTime)
		{
			currentStateIndex = i;
		}
	}
	int nextStateIndex = currentStateIndex + 1;
	if (nextStateIndex >= m_animationStates.size())
	{
		nextStateIndex = 0; // loops back around
	}

	VisemeData * currentVD, *nextVD;
	currentVD = &m_animationStates[currentStateIndex].data;
	nextVD = &m_animationStates[nextStateIndex].data;

	// calculate the lerp value
	float actualCutOffPoint = m_animationStates[currentStateIndex].endTime - m_timeToTransitionToNextState;

	bool needToLerp = m_secondsIntoAnimation >= actualCutOffPoint &&
		m_secondsIntoAnimation <= m_animationStates[currentStateIndex].endTime - m_timeToTransitionToNextState;

	float lerpVal = 0.0f; // will lerp anyway, the lerp value will be changed is need to change the shape
	// if lerpVal is 0.0f the apperence will just stay the same as the shape stored in  m_animationStates[currentStateIndex]

	if (needToLerp)
	{
		lerpVal = calculateLerpValue(actualCutOffPoint, m_animationStates[currentStateIndex].endTime, m_secondsIntoAnimation);
	}

	using namespace Math::interpolation;

	for (auto i = 0; i < m_activeAnimationState.size(); i++)
	{
		// std::vector<Vertex> vertDataToSendToD3D;
		for (unsigned int j = 0; j < currentVD->meshes[i].vertices.size(); j++)
		{
			Vertex v;
			v.pos.x = lerp(currentVD->meshes[i].vertices[j].pos.x, nextVD->meshes[i].vertices[j].pos.x, lerpVal);
			v.pos.y = lerp(currentVD->meshes[i].vertices[j].pos.y, nextVD->meshes[i].vertices[j].pos.y, lerpVal);
			v.pos.z = lerp(currentVD->meshes[i].vertices[j].pos.z, nextVD->meshes[i].vertices[j].pos.z, lerpVal);

			v.normal.x = lerp(currentVD->meshes[i].vertices[j].normal.x, nextVD->meshes[i].vertices[j].normal.x, lerpVal);
			v.normal.y = lerp(currentVD->meshes[i].vertices[j].normal.y, nextVD->meshes[i].vertices[j].normal.y, lerpVal);
			v.normal.z = lerp(currentVD->meshes[i].vertices[j].normal.z, nextVD->meshes[i].vertices[j].normal.z, lerpVal);

			m_vertexData[i][j] = v;
		}
		// now override the vetex buffer
		// the current vertex data stored in m_activeAnimationState[i].vertBuffer

		// going with the buffer altering approaches found on: http://www.braynzarsoft.net/viewtutorial/q16390-28-skeletal-animation-based-on-the-md5-format


		/* (how the buffer is declared)
		D3D11_BUFFER_DESC vbDesc;
		ZeroMemory(&vbDesc, sizeof(vbDesc));
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(Vertex) * nVerts;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		*/


		D3D11_MAPPED_SUBRESOURCE StageingVertBuffer;

		d3dDC->Map(m_activeAnimationState[i].vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &StageingVertBuffer);

		memcpy(StageingVertBuffer.pData, // copy data to
			&m_vertexData[i][0],  // copy data from
			(sizeof(Vertex) * m_vertexData[i].size())); // n bytes to copy

		d3dDC->Unmap(m_activeAnimationState[i].vertBuffer, 0);

		// vertDataToSendToD3D.clear();

		/* an alternative
		d3dDC->UpdateSubresource(m_activeAnimationState[i].vertBuffer, 0, NULL, &vertDataToSendToD3D[0], 0, 0);
		*/
	}

}

void VisemeAnimatedModel::shutdown()
{
	for (auto i = 0; i < m_activeAnimationState.size(); i++)
	{
		m_activeAnimationState[i].vertBuffer->Release();
		m_activeAnimationState[i].indexBuffer->Release();
	}
	m_vertexData.clear();
}

float VisemeAnimatedModel::calculateLerpValue(float min, float max, float x)
{
	// difference between min & max
	float diffMinMax = max - min;
	if (diffMinMax == 0.0f)
	{
		return 1.0f;
		// prevents division by 0.
		// the value would be after would be 1.0 anyway
	}

	// difference between min & point
	float diffMinX = x - min;
	// return diffMinPoint/diffMinMax
	return diffMinX / diffMinMax;
}