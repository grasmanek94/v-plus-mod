#include "Main.h"

uint64_t GameAddresses::ppPedFactory = NULL;
char * GameAddresses::pGameKeyArray = NULL;
uint64_t GameAddresses::scriptHandleToPed = NULL;
uint32_t GameAddresses::pedIntelligenceOffset = NULL;
uint8_t * GameAddresses::pMouseInputEnabled = NULL;
uint8_t * GameAddresses::pKeyboardInputEnabled = NULL;

bool GameAddresses::FindAddresses()
{
	auto location = GameUtility::FindPattern("\x8B\x87\x50\x01\x00\x00\xD1\xE8\xA8\x01\x74\x1E\x48\x8B\x05", "xxxxxxxxxxxxxxx");

	if(location)
	{
		ppPedFactory = (uint64_t)((location + 0x0F) + *(unsigned int *)(location + 0x0F) + 4);
	}
	else
	{
		return false;
	}

/*
	location = CGameUtility::FindPattern("\x40\x53\x48\x83\xEC\x30\x8B\x0D\x00\x00\x00\x00\x65\x48\x8B\x04\x25\x58\x00\x00\x00\xBA\xB4\x00\x00\x00\x48\x8B\x04\xC8\xB3\x01", "xxxxxxxx????xxxxxxxxxxxxxxxxxxxx");

	if(location)
	{
		pLastStateOfMultiplayerUI = (uint8_t *)((location + 0x3B) + *(unsigned int *)(location + 0x3B) + 4);
	}
	else
	{
		return false;
	}
*/

	location = GameUtility::FindPattern("\xBF\x00\x01\x00\x00\x48\x8D\x1D\x00\x00\x00\x00\x48\x3B\x05", "xxxxxxxx????xxx");

	if(location)
	{
		pGameKeyArray = (char *)((location + 0x08) + *(unsigned int *)(location + 0x08) + 4);
	}
	else
	{
		return false;
	}

	location = GameUtility::FindPattern("\x40\x53\x48\x83\xEC\x20\xE8\x00\x00\x00\x00\x48\x8B\xD8\x48\x85\xC0\x74\x00\x4C\x8B\x00\x8B\x15\x00\x00\x00\x00\x48\x8B\xC8\x41\xFF\x50\x08\x84\xC0\x74\x00\x48\x8B\xC3", "xxxxxxx????xxxxxxx?xxxxx????xxxxxxxxxx?xxx");

	if(location)
	{
		scriptHandleToPed = location;
	}
	else
	{
		return false;
	}

	location = GameUtility::FindPattern("\x57\x48\x83\xEC\x20\x33\xDB\xE8\x00\x00\x00\x00\x48\x8B\xF8\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\x4C\x8B\x80\x00\x00\x00\x00\x49\x8B\x00\x00\x00\x00\x00\x48\x85\xC0", "xxxxxxxx????xxxxxxxx????xxx????xx????xxx");

	if(location)
	{
		pedIntelligenceOffset = *(uint32_t *)(location + 27);
	}
	else
	{
		return false;
	}

	location = GameUtility::FindPattern("\x48\x83\xEC\x28\x80\x3D\x00\x00\x00\x00\x00\x74\x05\xE8\x00\x00\x00\x00\x80", "xxxxxx????xxxx????x");

	if(location)
	{
		pMouseInputEnabled = (uint8_t *)((location + 0x22) + *(unsigned int *)(location + 0x22) + 5);
		pKeyboardInputEnabled = (uint8_t *)((location + 0x14) + *(unsigned int *)(location + 0x14) + 5);
	}
	else
	{
		return false;
	}

	return true;
}
