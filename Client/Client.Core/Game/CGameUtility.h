#pragma once

class CGameUtility
{
public:
	static bool CompareMemory(const unsigned char *pData, const unsigned char *bMask, const char *sMask);
	static intptr_t FindPattern(const char *bMask, const char *sMask);

	static bool IsIntroMovieStringAvailable();
};
