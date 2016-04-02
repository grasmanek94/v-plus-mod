#include "DllMain.h"

uint64_t CGameAddresses::ppWorld = NULL;

bool CGameAddresses::FindAddresses()
{
	auto location = CGameUtility::FindPattern("\x8B\x87\x50\x01\x00\x00\xD1\xE8\xA8\x01\x74\x1E\x48\x8B\x05", "xxxxxxxxxxxxxxx");

	if(location)
	{
		ppWorld = (uint64_t)((location + 0x0F) + *(unsigned int *)(location + 0x0F) + 4);
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

	return true;
}
