#include "Main.h"

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/DirectX11.h"

bool GameOverlay::bInitialized = false;

ID3D11Device * GameOverlay::pDevice = NULL;
ID3D11DeviceContext * GameOverlay::pContext = NULL;

DXGISwapChainPresent GameOverlay::pRealPresent = NULL;
uintptr_t GameOverlay::hkSwapChainVFTable[64];

IFW1Factory * GameOverlay::pFW1Factory = NULL;
IFW1FontWrapper * GameOverlay::pFontWrapper = NULL;

ChatWindow *pChatWindow = NULL;

bool bGwenInitialized = false;
bool bGwenEnabled = false;

Gwen::Input::Windows GwenInput;

HRESULT __stdcall GameOverlay::HookedPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
	static Gwen::Renderer::DirectX11 *pRenderer = NULL;
	static Gwen::Skin::TexturedBase *pSkin = NULL;
	static Gwen::Controls::Canvas *pCanvas = NULL;
	static UnitTest *pUnitTest = NULL;

    if(!bInitialized)
    {
        pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
        pDevice->GetImmediateContext(&pContext);

        FW1CreateFactory(FW1_VERSION, &pFW1Factory);
        pFW1Factory->CreateFontWrapper(pDevice, L"Tahoma", &pFontWrapper);

        pFW1Factory->Release();

		if(pChatWindow == NULL)
		{
			pChatWindow = new ChatWindow(pContext, pFontWrapper);
		}

		HWND windowHandle = FindWindow(L"grcWindow", NULL);

		pRenderer = new Gwen::Renderer::DirectX11(pDevice);
		pSkin = new Gwen::Skin::TexturedBase(pRenderer);
		pSkin->Init("Skin.png");

		RECT FrameBounds;
		GetClientRect(windowHandle, &FrameBounds);

		pCanvas = new Gwen::Controls::Canvas(pSkin);
		pCanvas->SetSize(FrameBounds.right, FrameBounds.bottom);
/*
		pCanvas->SetDrawBackground(true);
		pCanvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));
*/
		pCanvas->SetDrawBackground(false);

		pUnitTest = new UnitTest(pCanvas);
		pUnitTest->SetPos(10, 10);

		GwenInput.Initialize(pCanvas);

		bGwenInitialized = true;
		bGwenEnabled = true;

        bInitialized = true;
    }

	/*wchar_t wszGameStateString[128];
	swprintf_s(wszGameStateString, 128, L"Game state: %d", GameStateWatcher::GetGameState());

	pFontWrapper->DrawString(pContext, wszGameStateString, 18.0f, 22.0f, 20.0f, 0xff000000, FW1_RESTORESTATE);
	pFontWrapper->DrawString(pContext, wszGameStateString, 18.0f, 20.0f, 18.0f, 0xffffffff, FW1_RESTORESTATE);

	if(GameStateWatcher::GetGameState() == GameStatePlaying && GameAddresses::ppPedFactory != NULL)
	{
		uint64_t pPedFactory = *(uint64_t *)(GameAddresses::ppPedFactory);

		if(pPedFactory != NULL)
		{
			uint64_t pLocalPlayerPed = *(uint64_t *)(pPedFactory + 8);

			if(pLocalPlayerPed != NULL)
			{
				wchar_t wszPositionString[128];
				swprintf_s(wszPositionString, 128, L"Position: %.2f, %.2f, %.2f",
					*(float *)(pLocalPlayerPed + 0x90), *(float *)(pLocalPlayerPed + 0x94), *(float *)(pLocalPlayerPed + 0x98));

				pFontWrapper->DrawString(pContext, wszPositionString, 18.0f, 22.0f, 42.0f, 0xff000000, FW1_RESTORESTATE);
				pFontWrapper->DrawString(pContext, wszPositionString, 18.0f, 20.0f, 40.0f, 0xffffffff, FW1_RESTORESTATE);
			}
		}
	}*/

	//drawTaskList(pContext, pFontWrapper);

	if(pChatWindow != NULL)
	{
		pChatWindow->Draw();
	}

	static eGameState lastGameState = GameStateUnknown;
	eGameState currentGameState = GameStateWatcher::GetGameState();

	if(lastGameState != currentGameState)
	{
		if(currentGameState == GameStatePlaying)
		{
			if(bGwenEnabled)
			{
				bGwenEnabled = false;
			}
		}

		lastGameState = currentGameState;
	}

	if(bGwenInitialized && bGwenEnabled && pCanvas != NULL)
	{
		pCanvas->RenderCanvas();
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
	if(pFontWrapper)
	{
		pFontWrapper->Release();
	}
}
