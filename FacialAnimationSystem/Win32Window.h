#ifndef _WIN32_WINDOW_H_
#define _WIN32_WINDOW_H_

#include <Windows.h>
#include <string>

class Win32Window
{
public:
	
	~Win32Window();

	HRESULT initWindow(HINSTANCE instanceHandle, int nCmdShow, UINT width, UINT height, std::string title);

	RECT getWindowRectangle();

	void shutdown();

	UINT getWindowWidth()
	{
		return m_windowWidth;
	}

	UINT getWindowHeight()
	{
		return m_windowHeight;
	}

	HWND getWindowHandle()
	{
		return m_windowHandle;
	}

	static Win32Window * getInstance();

	void processMessages();

	bool getIsOpen(){ return m_isOpen; }
	void setIsOpen(bool value) { m_isOpen = value; } // needed as part of the shutdown procedure

private:
	Win32Window(){ m_isOpen = true; } // uses Singlaton

	UINT m_windowWidth;
	UINT m_windowHeight;

	HINSTANCE m_applicationInstanceHandle;
	HWND m_windowHandle;

	bool m_isOpen;
};


#endif