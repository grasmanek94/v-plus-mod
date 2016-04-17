#pragma once

class GameAddresses
{
public:
	static uint64_t ppPedFactory;
	static char *pGameKeyArray;
	static uint64_t scriptHandleToPed;
	static uint32_t pedIntelligenceOffset;
	static uint8_t *pMouseInputEnabled;
	static uint8_t *pKeyboardInputEnabled;

public:
	static bool FindAddresses();
};
