#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <fstream>
#include <iostream>

#include "CustomConsole.hxx"

WNDPROC ConsoleWndProc = NULL;
HWND	ConsoleWndHandle = NULL;

void BindStdHandlesToConsole()
{
	/*//Redirect unbuffered STDIN to the console
	HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdInHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdInHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "r");
			if (file != NULL)
			{
				*stdin = *file;
				setvbuf(stdin, NULL, _IONBF, 0);
			}
		}
	}

	//Redirect unbuffered STDOUT to the console
	HANDLE stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdOutHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdOutHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if (file != NULL)
			{
				*stdout = *file;
				setvbuf(stdout, NULL, _IONBF, 0);
			}
		}
	}

	//Redirect unbuffered STDERR to the console
	HANDLE stdErrHandle = GetStdHandle(STD_ERROR_HANDLE);
	if (stdErrHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdErrHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if (file != NULL)
			{
				*stderr = *file;
				setvbuf(stderr, NULL, _IONBF, 0);
			}
		}
	}*/

	freopen("CONOUT$", "w", stdout);
	//Clear the error state for each of the C++ standard stream objects. We need to do this, as
	//attempts to access the standard streams before they refer to a valid target will cause the
	//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	//to always occur during startup regardless of whether anything has been read from or written to
	//the console or not.
	std::wcout.clear();
	std::cout.clear();
	/*std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();*/
}

void EnableCustomConsole()
{
	static bool CustomConsoleEnabled = false;

	if (CustomConsoleEnabled)
	{
		return;
	}

	CustomConsoleEnabled = true;

	// allocate a console for this app
	AllocConsole();
/*
	long long hConHandle;
	long long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 64;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stdout = *fp;

	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console

	lStdHandle = (long long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console
	lStdHandle = (long long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stderr = *fp;

	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
*/
	ConsoleWndHandle = GetConsoleWindow();
	ConsoleWndProc = (WNDPROC)GetWindowLongPtrA(ConsoleWndHandle, -4);

	BindStdHandlesToConsole();

	SetConsoleTitleA("GTA V - Dedicated Multiplayer");
}