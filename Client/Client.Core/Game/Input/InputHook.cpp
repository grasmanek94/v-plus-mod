#include "Main.h"

WNDPROC	oWndProc;

LRESULT APIENTRY InputHook::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);

	if(GameOverlay::IsInitialized() && GameOverlay::GetGameUI() != NULL)
	{
		GameOverlay::GetGameUI()->MsgProc(hwnd, uMsg, wParam, lParam);
	}

	return lResult;
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
