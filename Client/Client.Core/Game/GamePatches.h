#pragma once

class GamePatches
{
public:
	static bool InstallPatches();

private:
	static bool SkipIntro();
	static bool RevealFullMap(bool bToggle);
	static bool UseMultiplayerUI();
	static bool DisableWeaponAndRadioWheelEffect();
};
