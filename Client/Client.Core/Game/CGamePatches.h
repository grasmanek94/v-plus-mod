#pragma once

class CGamePatches
{
public:
	static bool InstallPatches();

private:
	static bool SkipIntro();
	static bool RevealFullMap(bool bToggle);
	static bool UseMultiplayerUI();
};
