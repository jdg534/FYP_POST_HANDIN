#include "Renderer.h"

#include <fstream>

#include "ErrorLog.h"

//#include <gl\GL.h>
//#include <gl\GLU.h>

#include "RenderingStructs.h"

#include "MathTypes.h"
//#include <GLFW\glfw3.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


bool matricesTheSame(DirectX::XMFLOAT4X4 a, Matrix4x4 b)
{
	
	if (a._11 != b.r1c1)
	{
		return false;
	}
	else if (a._12 != b.r1c2)
	{
		return false;
	}
	else if (a._13 != b.r1c3)
	{
		return false;
	}
	else if (a._14 != b.r1c4)
	{
		return false;
	}

	else if (a._21 != b.r2c1)
	{
		return false;
	}
	else if (a._22 != b.r2c2)
	{
		return false;
	}
	else if (a._23 != b.r2c3)
	{
		return false;
	}
	else if (a._24 != b.r2c4)
	{
		return false;
	}

	else if (a._31 != b.r3c1)
	{
		return false;
	}
	else if (a._32 != b.r3c2)
	{
		return false;
	}
	else if (a._33 != b.r3c3)
	{
		return false;
	}
	else if (a._34 != b.r3c4)
	{
		return false;
	}

	else if (a._41 != b.r4c1)
	{
		return false;
	}
	else if (a._42 != b.r4c2)
	{
		return false;
	}
	else if (a._43 != b.r4c3)
	{
		return false;
	}
	else if (a._44 != b.r4c4)
	{
		return false;
	}

	return true;
}


Renderer::Renderer()
{
	m_clearColourR = 0.0f;
	m_clearColourG = 0.0f;
	m_clearColourB = 0.0f;
	m_clearColourA = 1.0f;

	m_activeCam = nullptr;
	m_drawWireframe = true;
	m_windowText = "UNNAMED WINDOW";
	m_windowWidth = 800;
	m_windowHeight = 600;

	m_nearDepth = 0.1f;
	m_farDepth = 100.0f;

	//m_windowPtr = nullptr;
	m_windowOpen = false;

	m_fovAngle = 45.0f;
}

Renderer::~Renderer()
{

}

bool Renderer::init(std::string initFileName, HINSTANCE instanceHandle, int nCmdShow)
{
	ErrorLog * el = ErrorLog::getInstance();
	std::ifstream initFile(initFileName);
	if (!initFile.good())
	{
		el->logMinorError(initFileName + " wasn't found");
		return false;
	}

	std::string cb = ""; // CB CurrentBit
	while (cb != "END_RENDER_DESC")
	{
		initFile >> cb;
		if (cb == "windowWidth")
		{
			initFile >> m_windowWidth;
		}
		else if (cb == "windowHeight")
		{
			initFile >> m_windowHeight;
		}
		else if (cb == "windowText")
		{
			std::getline(initFile, m_windowText);
			// remove the quote marks at the start & end

			int firstQuote = m_windowText.find_first_of("\"");
			int lastQuote = m_windowText.find_last_of("\"");

			m_windowText = m_windowText.substr(firstQuote + 1, lastQuote - (firstQuote + 1));
		}
		else if (cb == "clearColour")
		{
			initFile >> m_clearColourR;
			initFile >> m_clearColourG;
			initFile >> m_clearColourB;
			initFile >> m_clearColourA;
		}
		else if (cb == "drawType")
		{
			std::string drawMode = "";
			initFile >> drawMode;
			if (drawMode == "\"wireframe\"")
			{
				m_drawWireframe = false;
			}
			else
			{
				m_drawWireframe = true;
			}
		}
		else if (cb == "nearDepth")
		{
			initFile >> m_nearDepth;
		}
		else if (cb == "farDepth")
		{
			initFile >> m_farDepth;
		}
	}


	// now create the window
	
	// Create Win32 window

	m_windowInstancePtr = Win32Window::getInstance();
	m_windowInstancePtr->initWindow(instanceHandle, nCmdShow, m_windowWidth, m_windowHeight, m_windowText);

	// init the renderer
	m_windowOpen = true;

	/* (Old code, pre move to D3D11)
	if (!glfwInit())
		return false;

	m_windowPtr = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowText.c_str(), NULL, NULL);
	if (!m_windowPtr)
	{
		return false;
	}


	glfwMakeContextCurrent(m_windowPtr);
	m_glewWindowOpen = true;

	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(0);

	*/

	// init the light

	m_activeLight.AmbientLightComponent = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_activeLight.DiffuseLightComponent = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_activeLight.SpecularLightComponent = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_activeLight.SpecularPower = 20.0f;
	m_activeLight.LightVecW = DirectX::XMFLOAT3(0.0f, 1.0f, -1.0f);
	m_activeLight.lightPosW = DirectX::XMFLOAT3(0.0f, -1.0f, -3.0f);


	return initD3D11Stuff();
}

bool Renderer::initD3D11Stuff()
{
	// just like what the function name says

	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	int nFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(scDesc));

	Win32Window * w = Win32Window::getInstance();

	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Width = w->getWindowWidth();
	scDesc.BufferDesc.Height = w->getWindowHeight();
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = w->getWindowHandle();
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, nFeatureLevels,
			D3D11_SDK_VERSION, &scDesc, &m_swapChainPtr, &m_d3dDevicePtr, &m_featureLevel, &m_d3dDeviceContextPtr);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;
	// if got past this point will have create a D3D device & it's context


	// now create the render target view
	ID3D11Texture2D * pBackBuffer = nullptr;

	hr = m_swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D),(LPVOID *) &pBackBuffer);
	if (FAILED(hr))
	{
		// coudn't get the back buffer
	}


	hr = m_d3dDevicePtr->CreateRenderTargetView(pBackBuffer, nullptr, &m_renderTargetViewPtr);
	if (hr == E_INVALIDARG)
	{
		// couldn't get the render target view
	}

	pBackBuffer->Release();
	// ERROR HERE!!!, fix it back home




	if (FAILED(hr))
	{
		return false;
	}

	// now the depth / stencil buffer
	D3D11_TEXTURE2D_DESC depthStenciBufDesc;

	depthStenciBufDesc.Width = w->getWindowWidth();
	depthStenciBufDesc.Height = w->getWindowHeight();
	depthStenciBufDesc.MipLevels = 1;
	depthStenciBufDesc.ArraySize = 1;
	depthStenciBufDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStenciBufDesc.SampleDesc.Count = 1;
	depthStenciBufDesc.SampleDesc.Quality = 0;
	depthStenciBufDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStenciBufDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStenciBufDesc.CPUAccessFlags = 0;
	depthStenciBufDesc.MiscFlags = 0;

	m_d3dDevicePtr->CreateTexture2D(&depthStenciBufDesc, nullptr, &m_depthStencilBuffer);
	m_d3dDevicePtr->CreateDepthStencilView(m_depthStencilBuffer, nullptr, &m_depthStencilView);

	// set the render target & the depth / stencil buffer
	m_d3dDeviceContextPtr->OMSetRenderTargets(1, &m_renderTargetViewPtr, m_depthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)w->getWindowWidth();
	vp.Height = (FLOAT)w->getWindowHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_d3dDeviceContextPtr->RSSetViewports(1, &vp);

	// create the rasterizer states
	hr = createRasterizerStates();

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create the rasterizer states", L"Couldn't create the rasterizer states", MB_OK);
	}

	hr = InitShadersAndInputLayout();

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to config Input Layout or Shader", L"Couldn't create the Input layout and or shader", MB_OK);
	}

	// Set primitive topology
	m_d3dDeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_d3dDevicePtr->CreateBuffer(&bd, nullptr, &m_constantBufferPtr);

	if (FAILED(hr))
		return hr;

	// DONE :)

	return true;
}

void Renderer::shutdown()
{
	// now deletes, just shutdown
	//glfwTerminate();

	// DirectX11 shutdown code here
}

void Renderer::startDrawing(Camera * drawWith)
{
	/*
	glClearColor(m_clearColourR, m_clearColourG, m_clearColourB, m_clearColourA);
	// glClearDepth(); // OpenGL defaults to 1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glfwGetFramebufferSize(m_windowPtr, (int *) &m_windowWidth, (int *) &m_windowHeight);

	glViewport(0, 0, m_windowWidth, m_windowHeight);


	glLoadIdentity();
	*/


	if (drawWith == nullptr)
	{
		return;
	}


	m_activeCam = drawWith;

	/*
	Vector3D eyePosition = drawWith->getEyePosition();
	Vector3D lookAtPosition = drawWith->getLookAtPosition();
	Vector3D upVector = drawWith->getUpVector();


	// deal with the projection matrix, then view matrix (Rember legacy openGL uses 4x4 matrix stack)
	float fieldOfViewY = 45.0f; // 45 Degrees
	float aspectRatio = (float) m_windowWidth / (float)m_windowHeight;
	//gluPerspective(fieldOfViewY, aspectRatio, m_nearDepth, m_farDepth);

	// set the view & projection matrices
	using namespace DirectX;
	XMMATRIX viewMatRH, viewMatLH,
		orthoProjectMatLH, prespectiveProjectMatLH,
		orthoProjectMatRH, prespectiveProjectMatRH;
	XMVECTOR eye, up, at;
	eye = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, 0.0f);
	up = XMVectorSet(upVector.x, upVector.y, upVector.z, 0.0f);
	at = XMVectorSet(lookAtPosition.x, lookAtPosition.y, lookAtPosition.z, 0.0f);

	viewMatLH = XMMatrixLookAtLH(eye, at, up);
	viewMatRH = XMMatrixLookAtRH(eye, at, up);

	prespectiveProjectMatLH = XMMatrixPerspectiveFovLH(XM_PIDIV2, aspectRatio, m_nearDepth, m_farDepth);
	prespectiveProjectMatRH = XMMatrixPerspectiveFovRH(XM_PIDIV2, aspectRatio, m_nearDepth, m_farDepth);

	orthoProjectMatLH = XMMatrixOrthographicLH((float)m_windowWidth, (float)m_windowHeight, m_nearDepth, m_farDepth);
	orthoProjectMatRH = XMMatrixOrthographicRH((float)m_windowWidth, (float)m_windowHeight, m_nearDepth, m_farDepth);
	
	XMStoreFloat4x4(&m_viewMatrix, viewMatLH);
	XMStoreFloat4x4(&m_projectionMatrix, prespectiveProjectMatLH);
	*/


	// temp, create own version of the XMFLOAT4X4 matrices, to see if own math library any good
	// using namespace Math::transform;
	
	//Matrix4x4 ownViewMatLH = calcLHViewMatrix(eyePosition, upVector, lookAtPosition);
	//Matrix4x4 ownPerspectiveMatLH = calculatePerspectiveProjectionMatrix(fieldOfViewY, aspectRatio, m_nearDepth, m_farDepth);

	// no compare the matrices
	//bool viewMatSame = matricesTheSame(m_viewMatrix, ownViewMatLH);
	//bool projectionMatSame = matricesTheSame(m_viewMatrix, ownViewMatLH);


	// clear the back buffer
	float clearColour[4] = {m_clearColourR, m_clearColourG, m_clearColourB, m_clearColourA};
	m_d3dDeviceContextPtr->ClearRenderTargetView(m_renderTargetViewPtr, clearColour);
	m_d3dDeviceContextPtr->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// set the Rasterizer to the solid 1
	m_d3dDeviceContextPtr->RSSetState(m_solidRasterizerStatePtr);

}

void Renderer::drawMesh(Mesh * toDraw)
{
	/* to test drawing code, Post own Matrix funcs
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	*/

	if (toDraw == nullptr)
	{
		return;
	}
	
	// update the device context
	m_d3dDeviceContextPtr->IASetInputLayout(m_vertexLayoutPtr);

	m_d3dDeviceContextPtr->VSSetShader(m_vertexShaderPtr, nullptr, 0);
	m_d3dDeviceContextPtr->PSSetShader(m_pixelShaderPtr, nullptr, 0);

	m_d3dDeviceContextPtr->VSSetConstantBuffers(0, 1, &m_constantBufferPtr);
	m_d3dDeviceContextPtr->PSSetConstantBuffers(0, 1, &m_constantBufferPtr);
	// if setting the sampler, do it here



	Vector3D eye = m_activeCam->getEyePosition();
	Vector3D at = m_activeCam->getLookAtPosition();
	Vector3D up = m_activeCam->getUpVector();

	// create the matrices
	using namespace DirectX;
	
	XMVECTOR vEye, vAt, vUp;
	vEye = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&eye));
	vAt = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&at));
	vUp = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&up));

	XMMATRIX world, view, projection;

	

	ConstantBuffer cb;

	Win32Window * w = Win32Window::getInstance();

	float aspectRatioHeightOverWidth = (float)w->getWindowHeight() / (float)w->getWindowWidth();

	// World Matrix
	world = toDraw->getWorldMatrix();
	cb.mWorld = world;

	// View Matrix
	view = XMMatrixLookAtLH(vEye, vAt, vUp);
	// now transpose it
	cb.mView = XMMatrixTranspose(view);

	

	// Projection Matrix
	projection = XMMatrixPerspectiveFovLH(m_fovAngle, aspectRatioHeightOverWidth, m_nearDepth, m_farDepth);
	// now transpose it
	cb.mProjection = XMMatrixTranspose(projection);

	

	// Material
	cb.mMaterial = toDraw->getMaterial();
	// light
	cb.mLight = m_activeLight;


	// Camera Position
	cb.eyePositionWorldSpace.x = eye.x;
	cb.eyePositionWorldSpace.y = eye.y;
	cb.eyePositionWorldSpace.z = eye.z;
	
	
	// update the actual constant buffer
	m_d3dDeviceContextPtr->UpdateSubresource(m_constantBufferPtr, 0, nullptr, &cb, 0, 0);


	toDraw->draw(m_d3dDeviceContextPtr);	
	
}

void Renderer::drawAnimatedMeshes(std::vector<AnimatedMesh> & meshesToDraw, DirectX::XMFLOAT4X4 * sharedWorldMatrixPtr)
{

	// update the device context
	m_d3dDeviceContextPtr->IASetInputLayout(m_vertexLayoutPtr);

	m_d3dDeviceContextPtr->VSSetShader(m_vertexShaderPtr, nullptr, 0);
	m_d3dDeviceContextPtr->PSSetShader(m_pixelShaderPtr, nullptr, 0);

	m_d3dDeviceContextPtr->VSSetConstantBuffers(0, 1, &m_constantBufferPtr);
	m_d3dDeviceContextPtr->PSSetConstantBuffers(0, 1, &m_constantBufferPtr);

	// the above lines set the input layout of each vertex and the vertex and pixel shader, as well as setting the constant buffer for both shaders


	Vector3D eye = m_activeCam->getEyePosition();
	Vector3D at = m_activeCam->getLookAtPosition();
	Vector3D up = m_activeCam->getUpVector();

	Win32Window * w = Win32Window::getInstance();

	float aspectRatioHeightOverWidth = (float)w->getWindowHeight() / (float)w->getWindowWidth();

	// create the matrices
	using namespace DirectX;

	XMVECTOR vEye, vAt, vUp;
	vEye = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&eye));
	vAt = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&at));
	vUp = XMLoadFloat3(reinterpret_cast<XMFLOAT3 *>(&up));

	XMMATRIX world, view, projection;

	world = XMLoadFloat4x4(sharedWorldMatrixPtr);

	view = XMMatrixLookAtLH(vEye, vAt, vUp);
	projection = projection = XMMatrixPerspectiveFovLH(m_fovAngle, aspectRatioHeightOverWidth, m_nearDepth, m_farDepth);

	ConstantBuffer cb;
	cb.mWorld = world;
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	XMStoreFloat3(&cb.eyePositionWorldSpace, vEye);

	cb.mLight = m_activeLight;

	for (auto i = 0; i < meshesToDraw.size(); i++)
	{
		cb.mMaterial = meshesToDraw[i].material;

		m_d3dDeviceContextPtr->UpdateSubresource(m_constantBufferPtr, 0, nullptr, &cb, 0, 0);

		
		m_d3dDeviceContextPtr->IASetVertexBuffers(0, 1, &meshesToDraw[i].vertBuffer, &meshesToDraw[i].vertexBufferStrideSize, &meshesToDraw[i].vertexBufferOffSet);
		m_d3dDeviceContextPtr->IASetIndexBuffer(meshesToDraw[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_d3dDeviceContextPtr->DrawIndexed(meshesToDraw[i].nIndercies, 0, 0);
	}
}

void Renderer::finshDrawing()
{
	// finsh drawing code here
	
	


	//glfwSwapBuffers(m_windowPtr);

	//glPopMatrix(); // pushed the new view matrix
	// glPopMatrix();
	
	// any post processing done here

	m_swapChainPtr->Present(0, 0);
}

void Renderer::checkWindow()
{
	// figure out if the GLFW window has been closed
	// glfwPollEvents();

	// check for window closing here
	/*
	if (!glfwWindowShouldClose(m_windowPtr)) // true is ment to keep the window open
	{
		m_glewWindowOpen = true;
	}
	else
	{
		m_glewWindowOpen = false;
	}
	*/
	
	m_windowInstancePtr->processMessages();

	m_windowOpen = m_windowInstancePtr->getIsOpen();
}

void Renderer::altGluPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
	/*
	// based off: http://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;

	//fH = tan( (fovY / 2) / 180 * pi ) * zNear;
	fH = tan(fovY / 360 * pi) * nearPlane;
	fW = fH * aspectRatio;

	// glFrustum(-fW, fW, -fH, fH, nearPlane, farPlane);
	glFrustum(-fW, fW, -fH, fH, nearPlane, farPlane);
	*/

	Matrix4x4 perspectiveMat = Math::transform::calculatePerspectiveProjectionMatrix(fovY, aspectRatio, nearPlane, m_farDepth);

	/*
	glLoadMatrixf(reinterpret_cast<GLfloat *>(&perspectiveMat));
	glPushMatrix();
	*/
}

void Renderer::altGluLookAtRH(Vector3D eyePos, Vector3D atPos, Vector3D upVec)
{
	/*
	// based off: http://www.3dgep.com/understanding-the-view-matrix/#The_View_Matrix

	using namespace Math::VectorMath;
	using namespace Math::MatrixMath;

	Matrix4x4 viewMat;
	makeIdentity(viewMat);

	Vector3D zAxis = unitVector(subtract(eyePos, atPos));
	Vector3D xAxis = unitVector(crossProduct(upVec, zAxis));
	Vector3D yAxis = crossProduct(zAxis, xAxis);

	Matrix4x4 translationMat;
	makeIdentity(translationMat);

	translationMat.r4c1 = -eyePos.x;
	translationMat.r4c2 = -eyePos.y;
	translationMat.r4c3 = -eyePos.z;

	Matrix4x4 finalViewMat = multiply(viewMat, translationMat);

	// now intro finalViewMat to OpenGL matrix Stack
	glLoadMatrixf(reinterpret_cast<GLfloat *>(&finalViewMat));
	glPushMatrix();

	*/

	Matrix4x4 viewMat = Math::transform::calcRHViewMatrix(eyePos, upVec, atPos);
	//glLoadMatrixf(reinterpret_cast<GLfloat *>(&viewMat));
	//glPushMatrix();
}

HRESULT Renderer::createRasterizerStates()
{
	HRESULT rv;
	D3D11_RASTERIZER_DESC solidRasterStateDesc;
	ZeroMemory(&solidRasterStateDesc, sizeof(solidRasterStateDesc));
	solidRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	solidRasterStateDesc.CullMode = D3D11_CULL_FRONT; // D3D11_CULL_FRONT or D3D11_CULL_NONE or D3D11_CULL_BACK
	// D3D11_CULL_FRONT from trial & error using the makeNormalCube func
	solidRasterStateDesc.FrontCounterClockwise = true;
	rv = m_d3dDevicePtr->CreateRasterizerState(&solidRasterStateDesc, &m_solidRasterizerStatePtr);

	if (FAILED(rv))
	{
		MessageBox(nullptr, L"Couldn't create rasterizer", L"Couldn't create solid rasterizer", MB_ICONEXCLAMATION);
		return rv;
	}

	D3D11_RASTERIZER_DESC wireFrameRasterStateDesc;
	ZeroMemory(&wireFrameRasterStateDesc, sizeof(wireFrameRasterStateDesc));
	wireFrameRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameRasterStateDesc.CullMode = D3D11_CULL_NONE;
	wireFrameRasterStateDesc.FrontCounterClockwise = true;
	rv = m_d3dDevicePtr->CreateRasterizerState(&wireFrameRasterStateDesc, &m_wireFrameRasterizerStatePtr);

	if (FAILED(rv))
	{
		MessageBox(nullptr, L"Couldn't create rasterizer", L"Couldn't create wire frame rasterizer", MB_ICONEXCLAMATION);
	}

	m_d3dDeviceContextPtr->RSSetState(m_solidRasterizerStatePtr);

	return rv;
}

HRESULT Renderer::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"CelShader.hlsl", "VertexShaderMain", "vs_4_0", &pVSBlob);



	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"Vertex Shader couldn't be compiled", L"Error", MB_OK);



		return hr;
	}

	// Create the vertex shader
	hr = m_d3dDevicePtr->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_vertexShaderPtr);

	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"CelShader.hlsl", "pixelShaderMain", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"Pixel Shader couldn't be compiled", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = m_d3dDevicePtr->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pixelShaderPtr);
	pPSBlob->Release();

	if (FAILED(hr))
		return hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = m_d3dDevicePtr->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_vertexLayoutPtr);
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	m_d3dDeviceContextPtr->IASetInputLayout(m_vertexLayoutPtr);

	return hr;
}

HRESULT Renderer::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
		{
			// OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

			// just find a way to put it in a string, then the the error log class
			char * outStr;
			outStr = (char*)pErrorBlob->GetBufferPointer();
			std::string outStrStringObjectForm = std::string(outStr);
			ErrorLog * el = ErrorLog::getInstance();

			std::wstring fileNameWStr(szFileName);
			std::string fileNameStr = "";
			fileNameStr.assign(fileNameWStr.begin(), fileNameWStr.end());

			el->logMinorError("Couldn't compile shader: " + fileNameStr);
			el->logFatalError(outStrStringObjectForm);



		}
		if (pErrorBlob) pErrorBlob->Release();

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}