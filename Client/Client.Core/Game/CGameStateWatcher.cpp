#include "DllMain.h"

eGameState * CGameStateWatcher::pGameState = NULL;

eGameState CGameStateWatcher::GetGameState()
{
	if(pGameState != NULL)
	{
		return *pGameState;
	}

	return GameStateUnknown;
}

DWORD WINAPI CGameStateWatcher::WorkerThread()
{
	for(;;)
	{
		eGameState gameState = GetGameState();

		if(gameState == GameStateRestartStory)
		{
			ExitProcess(0);
			return 0;
		}

		Sleep(50);
	}

	return 1;
}

bool CGameStateWatcher::Setup()
{
	auto gameStatePattern = CGameUtility::FindPattern("\x83\x3D\x00\x00\x00\x00\x00\x8A\xD9\x74\x0A", "xx?????xxxx");

	if(gameStatePattern == 0)
	{
		return false;
	}

	pGameState = (eGameState *)((gameStatePattern + 2) + *(unsigned int *)(gameStatePattern + 2) + 5);

	if(pGameState == NULL)
	{
		return false;
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WorkerThread, NULL, NULL, NULL);
	return true;
}
