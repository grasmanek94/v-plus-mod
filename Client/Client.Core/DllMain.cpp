#include "DllMain.h"

static TestThread testThread;

DWORD WINAPI Run()
{
	while(!CGameUtility::IsIntroMovieStringAvailable())
	{
		Sleep(50);
	}

	if(!InputHook::Initialize())
	{
		OutputDebugString(L"[V+ | Core] Failed to initialize input hook.");
		return 0;
	}

	CGameVersion::Check();

	if(CGameVersion::Get() == VER_1_0_678_1_STEAM || CGameVersion::Get() == VER_1_0_678_1_NOSTEAM)
	{
		CGamePatches::InstallPatches();
		CGameAddresses::FindAddresses();

		HWND windowHandle = FindWindow(L"grcWindow", NULL);
		SetWindowText(windowHandle, L"V+");

		CGameStateWatcher::Setup();
		CGameOverlay::Setup();
		CGameStartupManager::Setup();

		if(ScriptEngine::Initialize())
		{
			while(ScriptEngine::IsThreadCollectionEmpty())
			{
				Sleep(100);
			}

			ScriptEngine::CreateThread(&testThread);
		}
	}
	else
	{
		OutputDebugString(L"[V+ | Core] Unsupported game version.");

		ExitProcess(0);
		return 0;
	}

	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	srand((unsigned int)(time(0)));

	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Run, NULL, NULL, NULL);
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			CGameOverlay::Shutdown();
			break;
		}
	}

	return TRUE;
}

void TemporaryLogFunction(char *pszFormat, ...)
{
	char szBuffer[512];
	memset(szBuffer, 0, 512);

	va_list args;
	va_start(args, pszFormat);
	vsprintf_s(szBuffer, pszFormat, args);
	va_end(args);

	OutputDebugStringA(szBuffer);
}
