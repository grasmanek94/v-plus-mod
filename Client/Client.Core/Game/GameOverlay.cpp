#include "Main.h"

bool GameOverlay::bInitialized = false;
GameUI * GameOverlay::pGameUI = NULL;

DXGISwapChainPresent GameOverlay::pRealPresent = NULL;
uintptr_t GameOverlay::hkSwapChainVFTable[64];

HRESULT __stdcall GameOverlay::HookedPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    if(!bInitialized)
    {
		ID3D11Device *pD3D11Device = NULL;
		ID3D11DeviceContext *pD3D11DeviceContext = NULL;

        pSwapChain->GetDevice(__uuidof(pD3D11Device), (void**)&pD3D11Device);
        pD3D11Device->GetImmediateContext(&pD3D11DeviceContext);

		if(pGameUI == NULL)
		{
			pGameUI = new GameUI(pD3D11Device, pD3D11DeviceContext);
			pGameUI->Initialize();
		}

        bInitialized = true;
    }

	if(pGameUI != NULL && pGameUI->IsInitialized())
	{
		pGameUI->Draw();
	}

    return pRealPresent(pSwapChain, SyncInterval, Flags);
}

bool GameOverlay::Setup()
{
	auto location = GameUtility::FindPattern("\x80\x7E\x10\x00\x48\x8B\x0D\x00\x00\x00\x00\x48\x8D\x94\x24\xA0\x00\x00\x00\x0F\x94\x05", "xxxxxxx????xxxxxxxxxxx");

	while(!location)
	{
		location = GameUtility::FindPattern("\x80\x7E\x10\x00\x48\x8B\x0D\x00\x00\x00\x00\x48\x8D\x94\x24\xA0\x00\x00\x00\x0F\x94\x05", "xxxxxxx????xxxxxxxxxxx");

		Sleep(50);
	}

	if(location != 0)
	{
		uintptr_t ppDXGISwapChain = (uintptr_t)((location + 7) + *(unsigned int *)(location + 7) + 4);

		if(ppDXGISwapChain == NULL)
		{
			return false;
		}

		IDXGISwapChain *pSwapChain = *(IDXGISwapChain **)(ppDXGISwapChain);

		if(pSwapChain != NULL)
		{
			uintptr_t realSwapChainVFTable = *(uintptr_t *)(pSwapChain);
			DWORD dwProt1 = NULL, dwProt2 = NULL;

			VirtualProtect((LPVOID)(realSwapChainVFTable), 512, PAGE_EXECUTE_READWRITE, &dwProt1);
			memcpy(&hkSwapChainVFTable, (const void *)(realSwapChainVFTable), 512);
			VirtualProtect((LPVOID)(realSwapChainVFTable), 512, dwProt1, &dwProt2);

			pRealPresent = (DXGISwapChainPresent)hkSwapChainVFTable[8];
			hkSwapChainVFTable[8] = (uintptr_t)HookedPresent;

			VirtualProtect((LPVOID)(pSwapChain), 4, PAGE_EXECUTE_READWRITE, &dwProt1);
			*(uintptr_t *)(pSwapChain) = (uintptr_t)&hkSwapChainVFTable;
			VirtualProtect((LPVOID)(pSwapChain), 4, dwProt1, &dwProt2);
		}

		return true;
	}

	return false;
}

void GameOverlay::Shutdown()
{
	if(pGameUI != NULL)
	{
		delete pGameUI;
		pGameUI = NULL;
	}
}
