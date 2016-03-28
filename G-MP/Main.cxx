#include "..\OpenVHook\inc\main.h"
#include "Script.hxx"

void KeyboardHandlerFunction(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);
void GMP_Init();
void GMP_Cleanup();

void External_Init(HINSTANCE hInstance)
{
	//scriptRegister(hInstance, ScriptMain);
	//keyboardHandlerRegister(KeyboardHandlerFunction);
}

void External_Cleanup(HINSTANCE hInstance)
{
	//scriptUnregister(hInstance);
	//keyboardHandlerUnregister(KeyboardHandlerFunction);
}

#include <Windows.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for (; *mask; ++mask, ++data, ++pattern)
	{
		if (*mask == 'x' && *data != *pattern)
		{
			return false;
		}
	}
	return (*mask) == NULL;
}

// TODO: replace by OpenVHook Utility::Pattern and update code accordingly
UINT64 FindPattern(char *pattern, char *mask)
{	//Edited, From YSF by Kurta999
	UINT64 i;
	UINT64 size;
	UINT64 address;

	MODULEINFO info = { 0 };

	address = (UINT64)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (UINT64)info.SizeOfImage;

	for (i = 0; i < size; ++i)
	{
		if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
		{
			return (UINT64)(address + i);
		}
	}
	return 0;
}

void DisableRockstarLogos()
{
	UINT64 logos = FindPattern("platform:/movies/rockstar_logos", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if (logos != 0)
	{
		//memset((void*)(logos + 0x11), 0x00, 0x0E);
		memcpy((void*)logos, "./nonexistingfilenonexistingfil", 32);
	}
}

// Note to myself cuz 'never forgetti spaghetti': I found this by seeking a timer from 30000 ms to 0, which then makes GTA skip the legal screen [timer <= 0]
// Then I found where it jumps to the legal screen 'while loop' (well per frame shit) and just disabled that jump.
void DisableLegalMessagesCompletely()
{
	UINT64 address = FindPattern("\x72\x1F\xE8\x12\x8D\xFB\x00\x8B\x0D\x34\x7F\xC3\x01\xFF\xC1\x48", "xxx???xxx???xxxx");

	if (address != 0)
	{
		unsigned long dwProtect;
		unsigned long dwProtect2;

		VirtualProtect((LPVOID)address, 2, PAGE_EXECUTE_READWRITE, &dwProtect);
		*(unsigned short*)(address) = 0x9090;
		VirtualProtect((LPVOID)address, 2, dwProtect, &dwProtect2);
	}
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableRockstarLogos();
			DisableLegalMessagesCompletely();
			GMP_Init();
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			GMP_Cleanup();
			break;
		}
	}
	return TRUE;
}
