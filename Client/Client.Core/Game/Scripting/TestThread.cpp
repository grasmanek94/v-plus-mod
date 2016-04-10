#include "Main.h"

TestThread::TestThread()
{
	m_bInitialized = false;
	m_bIsWaitingForPedModelToLoad = false;
	m_bIsWaitingForVehicleModelToLoad = false;
	m_spawnedPeds.clear();
	m_bIsWaitingForClonePedModelToLoad = false;
	m_bClonePedSpawned = false;
	m_clonePed = -1;
}

eScriptThreadState TestThread::Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount)
{
	m_bInitialized = false;
	m_bIsWaitingForPedModelToLoad = false;
	m_bIsWaitingForVehicleModelToLoad = false;
	m_spawnedPeds.clear();
	m_bIsWaitingForClonePedModelToLoad = false;
	m_bClonePedSpawned = false;
	m_clonePed = -1;

	return ScriptThread::Reset(scriptHash, pArgs, argCount);
}

uint64_t g_pClonePed = NULL;

/*
struct CTaskPlayerOnFoot
{
private:
	uint8_t pad[480];
};
*/

extern uintptr_t hook::baseAddress;

void TestThread::DoRun()
{
	scrPed playerPedId = NativeInvoke::Invoke<GET_PLAYER_PED, uint32_t>(-1);

	if(playerPedId != -1 && playerPedId != 0)
	{
		if(!m_bInitialized)
		{
			NativeInvoke::Invoke<LOAD_SCENE, int>(-786.44f, -48.50f, 37.75f);

			NativeInvoke::Invoke<SHUTDOWN_LOADING_SCREEN, int>();
			NativeInvoke::Invoke<DO_SCREEN_FADE_IN, int>(0);

			NativeInvoke::Invoke<SET_GARBAGE_TRUCKS, int>(0);
			NativeInvoke::Invoke<SET_RANDOM_BOATS, int>(0);
			NativeInvoke::Invoke<SET_RANDOM_TRAINS, int>(0);

			NativeInvoke::Invoke<SET_ENTITY_COORDS, int>(playerPedId, -786.44f, -48.50f, 37.75f);

/*
			uintptr_t baseAddress = (uintptr_t)GetModuleHandle(nullptr);
			FILE *pNativeHandlerListFile = fopen("nativeHandlers.txt", "w");

			if(pNativeHandlerListFile)
			{
				fprintf(pNativeHandlerListFile, "base: 0x%p\n\n", baseAddress);

				for(uint32_t i = 0; i < 10514; i += 2)
				{
					ScriptEngine::NativeHandler nativeHandler = ScriptEngine::GetNativeHandler(nativeTranslationTable_335to350[i]);
					uintptr_t relativeAddress = (uintptr_t)nativeHandler - baseAddress;

					fprintf(pNativeHandlerListFile, "0x%p --> 0x%p (0x%p)\n", nativeTranslationTable_335to350[i], nativeHandler, relativeAddress);
				}

				fclose(pNativeHandlerListFile);
			}
*/

			m_bInitialized = true;
		}

		static bool s_bWasF9Pressed = true;
		static bool s_bWasF10Pressed = true;
		static bool s_bWasF11Pressed = true;
		static bool s_bWasF4Pressed = true;

		if(GetAsyncKeyState(VK_F9) & 0x8000)
		{
			if(!s_bWasF9Pressed)
			{
				NativeInvoke::Invoke<REQUEST_MODEL, int>(0xC1AE4D16);

				m_bIsWaitingForVehicleModelToLoad = true;
				s_bWasF9Pressed = true;
			}
		}
		else
		{
			s_bWasF9Pressed = false;
		}

		if(GetAsyncKeyState(VK_F10) & 0x8000)
		{
			if(!s_bWasF10Pressed)
			{
				if(NativeInvoke::Invoke<DOES_ENTITY_EXIST, uint32_t>(playerPedId) != 0)
				{
					static LPCSTR weaponNames[] = {
						"WEAPON_KNIFE", "WEAPON_NIGHTSTICK", "WEAPON_HAMMER", "WEAPON_BAT", "WEAPON_GOLFCLUB", "WEAPON_CROWBAR", 
						"WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50", "WEAPON_MICROSMG", "WEAPON_SMG", 
						"WEAPON_ASSAULTSMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_ADVANCEDRIFLE", "WEAPON_MG",
						"WEAPON_COMBATMG", "WEAPON_PUMPSHOTGUN", "WEAPON_SAWNOFFSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN",
						"WEAPON_STUNGUN", "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_GRENADELAUNCHER", "WEAPON_GRENADELAUNCHER_SMOKE",
						"WEAPON_RPG", "WEAPON_MINIGUN", "WEAPON_GRENADE", "WEAPON_STICKYBOMB", "WEAPON_SMOKEGRENADE", "WEAPON_BZGAS",
						"WEAPON_MOLOTOV", "WEAPON_FIREEXTINGUISHER", "WEAPON_PETROLCAN",
						"WEAPON_SNSPISTOL", "WEAPON_SPECIALCARBINE", "WEAPON_HEAVYPISTOL", "WEAPON_BULLPUPRIFLE", "WEAPON_HOMINGLAUNCHER",
						"WEAPON_PROXMINE", "WEAPON_SNOWBALL", "WEAPON_VINTAGEPISTOL", "WEAPON_DAGGER", "WEAPON_FIREWORK", "WEAPON_MUSKET",
						"WEAPON_MARKSMANRIFLE", "WEAPON_HEAVYSHOTGUN", "WEAPON_GUSENBERG", "WEAPON_HATCHET", "WEAPON_RAILGUN"
					};

					uint32_t hash = NativeInvoke::Invoke<GET_HASH_KEY, uint32_t>("WEAPON_SMG");
					NativeInvoke::Invoke<GIVE_DELAYED_WEAPON_TO_PED, uint32_t>(playerPedId, hash, 1000, 0);

					for(int i = 0; i < sizeof(weaponNames) / sizeof(weaponNames[0]); i++)
					{
						uint32_t hash = NativeInvoke::Invoke<GET_HASH_KEY, uint32_t>((char *)weaponNames[i]);

						NativeInvoke::Invoke<GIVE_DELAYED_WEAPON_TO_PED, uint32_t>(playerPedId, hash, 1000, 0);
					}

					NativeInvoke::Invoke<SET_NOTIFICATION_TEXT_ENTRY, int>("STRING");
					NativeInvoke::Invoke<ADD_TEXT_COMPONENT_STRING, int>("All weapons added.");
					NativeInvoke::Invoke<DRAW_NOTIFICATION, int>(FALSE, FALSE);
				}

				s_bWasF10Pressed = true;
			}
			else
			{
				s_bWasF10Pressed = false;
			}
		}

		if(GetAsyncKeyState(VK_F11) & 0x8000)
		{
			if(!s_bWasF11Pressed)
			{
				NativeInvoke::Invoke<REQUEST_MODEL, int>(0x65B93076);

				m_bIsWaitingForPedModelToLoad = true;
				s_bWasF11Pressed = true;
			}
		}
		else
		{
			s_bWasF11Pressed = false;
		}

		if(m_bIsWaitingForVehicleModelToLoad)
		{
			if(NativeInvoke::Invoke<HAS_MODEL_LOADED, bool>(0xC1AE4D16))
			{
				scrVector entityCoords = NativeInvoke::Invoke<GET_ENTITY_COORDS, scrVector>(playerPedId);

				NativeInvoke::Invoke<CREATE_VEHICLE, int>(0xC1AE4D16, entityCoords.x, entityCoords.y + 2, entityCoords.z, 0.0f, 1, 0);

				NativeInvoke::Invoke<SET_NOTIFICATION_TEXT_ENTRY, int>("STRING");
				NativeInvoke::Invoke<ADD_TEXT_COMPONENT_STRING, int>("Vehicle spawned.");
				NativeInvoke::Invoke<DRAW_NOTIFICATION, int>(FALSE, FALSE);

				m_bIsWaitingForVehicleModelToLoad = false;
			}
		}

		if(m_bIsWaitingForPedModelToLoad)
		{
			if(NativeInvoke::Invoke<HAS_MODEL_LOADED, bool>(0x65B93076))
			{
				scrVector entityCoords = NativeInvoke::Invoke<GET_ENTITY_COORDS, scrVector>(playerPedId);
				scrPed ped = NativeInvoke::Invoke<CREATE_PED, uint32_t>(1, 0x65B93076, entityCoords.x, entityCoords.y + 2, entityCoords.z, 0.0f, 0, 1);

				NativeInvoke::Invoke<SET_PED_CAN_BE_TARGETTED, int>(ped, 1);

				NativeInvoke::Invoke<SET_NOTIFICATION_TEXT_ENTRY, int>("STRING");
				NativeInvoke::Invoke<ADD_TEXT_COMPONENT_STRING, int>("Ped spawned.");
				NativeInvoke::Invoke<DRAW_NOTIFICATION, int>(FALSE, FALSE);

				m_spawnedPeds.push_back(ped);
				m_bIsWaitingForPedModelToLoad = false;
			}
		}

		if(GetAsyncKeyState(VK_F4) & 0x8000)
		{
			if(!s_bWasF11Pressed)
			{
				if(!m_bIsWaitingForClonePedModelToLoad && !m_bClonePedSpawned)
				{
					NativeInvoke::Invoke<REQUEST_MODEL, int>(0x9B810FA2);

					m_bIsWaitingForClonePedModelToLoad = true;
				}

				s_bWasF4Pressed = true;
			}
		}
		else
		{
			s_bWasF4Pressed = false;
		}

		if(m_bIsWaitingForClonePedModelToLoad)
		{
			if(NativeInvoke::Invoke<HAS_MODEL_LOADED, bool>(0x9B810FA2))
			{
				scrVector entityCoords = NativeInvoke::Invoke<GET_ENTITY_COORDS, scrVector>(playerPedId);

				m_clonePed = NativeInvoke::Invoke<CREATE_PED, uint32_t>(1, 0x9B810FA2, entityCoords.x, entityCoords.y + 2, entityCoords.z, 0.0f, 0, 1);

				/*if(GameAddresses::scriptHandleToPed != NULL)
				{
					typedef uint64_t (__fastcall * scriptHandleToPed_t)(scrPed pedHandle);
					static scriptHandleToPed_t scriptHandleToPed = (scriptHandleToPed_t)GameAddresses::scriptHandleToPed;

					uint64_t pClonePed = scriptHandleToPed(m_clonePed);

					if(pClonePed != NULL && GameAddresses::pedIntelligenceOffset != 0)
					{
						uint64_t pClonePedIntelligence = *(uint64_t *)(pClonePed + GameAddresses::pedIntelligenceOffset);

						if(pClonePedIntelligence != NULL)
						{
							uint64_t pTaskTreePed = *(uint64_t *)(pClonePedIntelligence + 0x360);

							if(pTaskTreePed != NULL)
							{
								uint64_t taskPlayerOnFootConstructorAddr = hook::baseAddress + 0xA35278; // 678
								CTaskPlayerOnFoot *pTaskPlayerOnFoot = new CTaskPlayerOnFoot;

								typedef uint64_t (__fastcall * taskPlayerOnFootConstructor_t)(CTaskPlayerOnFoot *pTaskPlayerOnFoot);
								static taskPlayerOnFootConstructor_t taskPlayerOnFootConstructor = (taskPlayerOnFootConstructor_t)taskPlayerOnFootConstructorAddr;

								taskPlayerOnFootConstructor(pTaskPlayerOnFoot);

								(*(void (__fastcall **)(DWORD64, DWORD64, signed __int64, DWORD64))(*(DWORD64 *)(pTaskTreePed) + 16))(pTaskTreePed, (DWORD64)pTaskPlayerOnFoot, 4, 0);
							}
						}
					}
				}*/

				m_bClonePedSpawned = true;
				m_bIsWaitingForClonePedModelToLoad = false;
			}
		}

		if(m_bClonePedSpawned && GameAddresses::scriptHandleToPed != NULL && m_clonePed != -1)
		{
			if(NativeInvoke::Invoke<DOES_ENTITY_EXIST, bool>(m_clonePed) && NativeInvoke::Invoke<IS_ENTITY_A_PED, bool>(m_clonePed))
			{
				if(!NativeInvoke::Invoke<IS_ENTITY_DEAD, bool>(m_clonePed) && !NativeInvoke::Invoke<IS_PED_DEAD, bool>(m_clonePed))
				{
					if(!NativeInvoke::Invoke<IS_ENTITY_DEAD, bool>(playerPedId) && !NativeInvoke::Invoke<IS_PED_DEAD, bool>(playerPedId))
					{
						typedef uint64_t (__fastcall * scriptHandleToPed_t)(scrPed pedHandle);
						static scriptHandleToPed_t scriptHandleToPed = (scriptHandleToPed_t)GameAddresses::scriptHandleToPed;

						uint64_t pLocalPlayerPed = scriptHandleToPed(playerPedId);
						uint64_t pClonePed = scriptHandleToPed(m_clonePed);

						if(pLocalPlayerPed != NULL && pClonePed != NULL)
						{
							g_pClonePed = pClonePed;

							scrVector playerCoords;
							playerCoords.x = *(float *)(pLocalPlayerPed + 0x90);
							playerCoords.y = *(float *)(pLocalPlayerPed + 0x94);
							playerCoords.z = *(float *)(pLocalPlayerPed + 0x98);

							scrVector playerRotation = NativeInvoke::Invoke<GET_ENTITY_ROTATION, scrVector>(playerPedId, 0);
							scrVector playerVelocity = NativeInvoke::Invoke<GET_ENTITY_VELOCITY, scrVector>(playerPedId, 0);

							if(NativeInvoke::Invoke<IS_PED_JUMPING, bool>(playerPedId) && !NativeInvoke::Invoke<IS_PED_JUMPING, bool>(m_clonePed))
							{
								playerCoords.z += 0.075f;
							}

							NativeInvoke::Invoke<SET_ENTITY_COORDS_NO_OFFSET, int>(m_clonePed, playerCoords.x, playerCoords.y + 2, playerCoords.z);
							NativeInvoke::Invoke<SET_ENTITY_ROTATION, int>(m_clonePed, playerRotation.x, playerRotation.y, playerRotation.z, 2, 1);
							NativeInvoke::Invoke<SET_ENTITY_VELOCITY, int>(m_clonePed, playerVelocity.x, playerVelocity.y, playerVelocity.z);

							scrHash
								curPlayerPedWeaponHash = 0,
								curClonePedWeaponHash = 0;
						
							NativeInvoke::Invoke<GET_CURRENT_PED_WEAPON, scrHash>(playerPedId, &curPlayerPedWeaponHash, true);
							NativeInvoke::Invoke<GET_CURRENT_PED_WEAPON, scrHash>(m_clonePed, &curClonePedWeaponHash, true);

							if(curPlayerPedWeaponHash != curClonePedWeaponHash)
							{
								NativeInvoke::Invoke<GIVE_WEAPON_TO_PED, scrVoid>(m_clonePed, curPlayerPedWeaponHash, 9999, true, true);
								NativeInvoke::Invoke<SET_CURRENT_PED_WEAPON, scrVoid>(m_clonePed, curPlayerPedWeaponHash, true);
							}

							if(NativeInvoke::Invoke<IS_PED_RELOADING, bool>(playerPedId) && !NativeInvoke::Invoke<IS_PED_RELOADING, bool>(m_clonePed))
							{
								NativeInvoke::Invoke<TASK_RELOAD_WEAPON, scrVoid>(m_clonePed, 1); // not seems to work
							}

							if(NativeInvoke::Invoke<IS_PED_JUMPING, bool>(playerPedId) && !NativeInvoke::Invoke<IS_PED_JUMPING, bool>(m_clonePed))
							{
								NativeInvoke::Invoke<TASK_JUMP, scrVoid>(m_clonePed, true);
							}
							else if(playerVelocity.x > 0 || playerVelocity.y > 0 || playerVelocity.z > 0)
							{
								NativeInvoke::Invoke<TASK_GO_STRAIGHT_TO_COORD, scrVoid>(m_clonePed, playerCoords.x, playerCoords.y + 2, playerCoords.z, 4.0f, 15, 0.0f, 0.0f);
							}
							else
							{
								NativeInvoke::Invoke<TASK_STAND_STILL, scrVoid>(m_clonePed, 15);
							}

							*(float *)(pClonePed + 0x578) = *(float *)(pLocalPlayerPed + 0x578); // current sprinting speed
							*(float *)(pClonePed + 0x580) = *(float *)(pLocalPlayerPed + 0x580); // target sprinting speed
						}
					}
				}
				else
				{
					NativeInvoke::Invoke<RESURRECT_PED, int>(m_clonePed);
					NativeInvoke::Invoke<CLEAR_PED_TASKS_IMMEDIATELY, scrVoid>(m_clonePed);
/*
					g_pClonePed = NULL;

					scrPed handle = m_clonePed;

					m_bClonePedSpawned = false;
					m_clonePed = -1;

					NativeInvoke::Invoke<DELETE_PED, scrVoid>(&handle);
*/
				}
			}
		}

		NativeInvoke::Invoke<SET_PED_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0f);
		NativeInvoke::Invoke<SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0f, 0.0f);

		NativeInvoke::Invoke<SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0f);
		NativeInvoke::Invoke<SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0f);
		NativeInvoke::Invoke<SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0f);

		NativeInvoke::Invoke<SUPPRESS_SHOCKING_EVENTS_NEXT_FRAME, uint32_t>();
		NativeInvoke::Invoke<SUPPRESS_AGITATION_EVENTS_NEXT_FRAME, uint32_t>();

		NativeInvoke::Invoke<SET_MAX_WANTED_LEVEL, int>(0);
	}
}
