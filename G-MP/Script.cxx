#include "script.hxx"
#include "CustomConsole.hxx"
#include "KeyboardManager.hxx"

#include "GTAV.hxx"

#include <Windows.h>

#pragma comment(lib, "Shell32.lib")

//CMultiplayer * Multiplayer = new CMultiplayer();

GTA::V* game = nullptr;
GTA::CustomHelpText* cht = nullptr;
bool activated = false;

void OnKeyStateChange(bool pressed, unsigned char vkey)
{
	if (pressed)
	{
		const std::vector<unsigned char> control_pattern({VK_CONTROL, VK_CONTROL, 'K', 'K', VK_RETURN});

		if (activated && vkey == VK_RETURN)
		{
			activated = false;
			cht->End();
			kbmgr.ClearBuffer();
			kbmgr.BackspaceTextEditing = false;
		}
		else if (!activated && kbmgr.BufferContainsArray(control_pattern))
		{
			activated = true;
			
			if (activated)
			{
				cht->Begin();
				kbmgr.ClearBuffer();
				kbmgr.BackspaceTextEditing = true;
			}
		}

		if (activated)
		{
			cht->SetText(std::string("IP:PORT/HOST? -> ") + (char*)kbmgr.GetSequence(128, false).data());
		}
	}
}

void main()
{
	//EnableCustomConsole();

	game = GTA::V::Setup();
	//cht = new GTA::CustomHelpText("GZ-LGM| ");

	printf("Game Base Addr: 0x%p\r\n", game);

	kbmgr.SetOnKeyStateChangeFunction(OnKeyStateChange);

	KeyboardManager::TACS safetyMechanism;
	while (true)
	{
		while (kbmgr.AntiCrashSychronization.try_pop(safetyMechanism))
		{
			kbmgr.CheckKeys(safetyMechanism.first, safetyMechanism.second);
		}

		//Multiplayer->Process();
		//if(kbmgr.Down(VK_SHIFT) && kbmgr.Pressed(VK_RETURN))
		//{
		//	std::cout << "Starting LAN Server... ";
		//	std::cout << Multiplayer->Start(true, "0.0.0.0", 45678) << std::endl;
		//}

		if (activated)
		{
			PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
			PED::SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0, 0.0);

			VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
			VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
			VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);

			PLAYER::SET_MAX_WANTED_LEVEL(0);

			//cht->ShowThisFrame();
		}
		
		WAIT(0);
	}
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}