#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>
#include <vector>


// #include <GLFW\glfw3.h>
//#include <gl\GL.h>
//#include <gl\GLU.h>


#include "Camera.h"
#include "Mesh.h"

#include "Math.h"


#include <d3d11.h>
#include <DirectXMath.h>

#include "Win32Window.h"

#include "VisemeAnimatedModel.h"

class Renderer
{
public:
	Renderer();

	~Renderer();

	bool init(std::string initFileName, HINSTANCE instanceHandle, int nCmdShow);
	
	bool initD3D11Stuff();
	
	void shutdown();

	void startDrawing(Camera * drawWith);
	void drawMesh(Mesh * toDraw);
	void drawAnimatedMeshes(std::vector<AnimatedMesh> & meshesToDraw, DirectX::XMFLOAT4X4 * sharedWorldMatrixPtr);
	void finshDrawing();



	bool isWindowOpen()
	{
		return m_windowOpen;
	}

	void checkWindow();


	ID3D11Device * getD3DDevicePtr(){ return m_d3dDevicePtr; }
	ID3D11DeviceContext * getD3DDeviceContextPtr(){ return m_d3dDeviceContextPtr; }

	void setActiveLight(Light l)
	{
		m_activeLight = l;
	}

	

private:
	unsigned int m_windowWidth;
	unsigned int m_windowHeight;
	std::string m_windowText;

	float m_nearDepth;
	float m_farDepth;

	bool m_drawWireframe;
	float m_clearColourR, m_clearColourG, m_clearColourB, m_clearColourA;
	// GLFWwindow * m_windowPtr;

	float m_fovAngle;

	bool m_windowOpen;

	Camera * m_activeCam;

	Light m_activeLight;

	Win32Window * m_windowInstancePtr;

	void altGluPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane);
	void altGluLookAtRH(Vector3D eyePos, Vector3D atPos, Vector3D upVec);


	// below is the D3D11 stuff
	D3D_DRIVER_TYPE         m_driverType;
	D3D_FEATURE_LEVEL       m_featureLevel;
	ID3D11Device*           m_d3dDevicePtr;
	ID3D11DeviceContext*    m_d3dDeviceContextPtr;
	IDXGISwapChain*         m_swapChainPtr;
	ID3D11RenderTargetView* m_renderTargetViewPtr;

	ID3D11VertexShader*     m_vertexShaderPtr;
	ID3D11PixelShader*      m_pixelShaderPtr;
	ID3D11InputLayout*      m_vertexLayoutPtr;

	ID3D11Buffer*           m_constantBufferPtr;

	ID3D11DepthStencilView * m_depthStencilView;

	ID3D11RasterizerState * m_solidRasterizerStatePtr;
	ID3D11RasterizerState * m_wireFrameRasterizerStatePtr;



	ID3D11Texture2D * m_depthStencilBuffer;

	// now the D3D11 functions

	HRESULT createRasterizerStates();
	HRESULT InitShadersAndInputLayout();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);


	//DirectX::XMFLOAT4X4 m_viewMatrix;
	//DirectX::XMFLOAT4X4 m_projectionMatrix;

};


#endif 