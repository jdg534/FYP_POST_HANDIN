#include "Win32Window.h"

#include "GlobalMessageQueue.h"

Win32Window * f_instancePtr = nullptr;

LRESULT CALLBACK WindowsCallbackFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		GlobalMessageQueue * globalMessageQueuePtr;
		globalMessageQueuePtr = GlobalMessageQueue::getInstance();
		globalMessageQueuePtr->addMessage(MT_QUIT);
		PostQuitMessage(0);
		f_instancePtr->setIsOpen(false);
		

		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Win32Window::~Win32Window()
{
	f_instancePtr = nullptr;
}

HRESULT Win32Window::initWindow(HINSTANCE instanceHandle, int nCmdShow, UINT width, UINT height, std::string title)
{
	HRESULT rv;

	WNDCLASSEX extentedWindowsClass;
	extentedWindowsClass.cbSize = sizeof(WNDCLASSEX);
	extentedWindowsClass.style = CS_HREDRAW | CS_VREDRAW;
	extentedWindowsClass.lpfnWndProc = WindowsCallbackFunc;
	extentedWindowsClass.cbClsExtra = 0;
	extentedWindowsClass.cbWndExtra = 0;
	extentedWindowsClass.hInstance = instanceHandle;
	extentedWindowsClass.hIcon = LoadIcon(instanceHandle, IDI_APPLICATION);
	extentedWindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	extentedWindowsClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	extentedWindowsClass.lpszMenuName = nullptr;
	extentedWindowsClass.lpszClassName = L"DX11_WINDOW_CLASS";
	extentedWindowsClass.hIconSm = LoadIcon(extentedWindowsClass.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&extentedWindowsClass))
	{
		return E_FAIL;
	}

	// now just get the window visable
	m_windowWidth = width;
	m_windowHeight = height;

	m_applicationInstanceHandle = instanceHandle;

	RECT r = { 0, 0, width, height };
	AdjustWindowRect(&r, WS_OVERLAPPED, FALSE);

	//LPSTR winTitle = 

	std::wstring wstrTitle = L"";
	wstrTitle.assign(title.begin(), title.end());

	m_windowHandle = CreateWindow(
		L"DX11_WINDOW_CLASS",
		wstrTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, nullptr, nullptr, m_applicationInstanceHandle,
		nullptr);
	if (!m_windowHandle)
	{
		return E_FAIL;
	}
	ShowWindow(m_windowHandle, nCmdShow);
	return S_OK;
}

RECT Win32Window::getWindowRectangle()
{
	RECT rv;
	GetWindowRect(m_windowHandle, &rv);
	return rv;
}

void Win32Window::shutdown()
{
	DestroyWindow(m_windowHandle);
}

Win32Window * Win32Window::getInstance()
{
	if (f_instancePtr == nullptr)
	{
		f_instancePtr = new Win32Window();
	}
	return f_instancePtr;
}

void Win32Window::processMessages()
{
	MSG osMessage;
	// while (GetMessageW(&osMessage, m_windowHandle, 0,0))
	while (PeekMessage(&osMessage, m_windowHandle, 0, 0, 1))
	{
		TranslateMessage(&osMessage);
		DispatchMessage(&osMessage);
	}
}