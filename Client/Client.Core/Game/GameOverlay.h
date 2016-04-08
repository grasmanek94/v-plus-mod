#pragma once

typedef HRESULT (__stdcall *DXGISwapChainPresent) (IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags); 

class GameOverlay
{
private:
	static bool bInitialized;

	static ID3D11Device *pDevice;
	static ID3D11DeviceContext *pContext;

	static DXGISwapChainPresent pRealPresent;
	static uintptr_t hkSwapChainVFTable[64];

	static IFW1Factory *pFW1Factory;
	static IFW1FontWrapper *pFontWrapper;

	static HRESULT __stdcall HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

public:
	static bool Setup();
	static void Shutdown();
};
