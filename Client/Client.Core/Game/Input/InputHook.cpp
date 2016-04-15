#include "Main.h"
#include "Gwen/Input/Windows.h"

WNDPROC	oWndProc;

extern bool bGwenInitialized;
extern bool bGwenEnabled;

extern Gwen::Input::Windows GwenInput;

LRESULT APIENTRY InputHook::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(bGwenInitialized)
	{
		if(uMsg == WM_KEYUP && wParam == VK_F2)
		{
			bGwenEnabled ^= 1;
		}

		if(bGwenEnabled)
		{
			MSG msg;

			msg.hwnd = hwnd;
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;

			GwenInput.ProcessMessage(msg);
		}
	}

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}

bool InputHook::Initialize()
{
	HWND windowHandle = NULL;

	while(windowHandle == NULL)
	{
		windowHandle = FindWindow(L"grcWindow", NULL);

		Sleep(100);
	}

	oWndProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)WndProc);
	return (oWndProc != NULL);
}

void InputHook::Remove()
{
	HWND windowHandle = FindWindow(L"grcWindow", NULL);
	SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}
