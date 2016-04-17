#pragma once

typedef HRESULT (__stdcall *DXGISwapChainPresent) (IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags); 

class GameUI;

class GameOverlay
{
private:
	static bool bInitialized;
	static GameUI *pGameUI;

	static DXGISwapChainPresent pRealPresent;
	static uintptr_t hkSwapChainVFTable[64];

	static HRESULT __stdcall HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

public:
	static bool IsInitialized() { return bInitialized; }
	static GameUI * GetGameUI() { return pGameUI; }

	static bool Setup();
	static void Shutdown();
};
