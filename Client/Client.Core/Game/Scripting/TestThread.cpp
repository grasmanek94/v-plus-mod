#include "DllMain.h"

TestThread::TestThread()
{
	m_bInitialized = false;
	m_bIsWaitingForModelToLoad = false;
}

eScriptThreadState TestThread::Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount)
{
	m_bInitialized = false;
	m_bIsWaitingForModelToLoad = false;

	return ScriptThread::Reset(scriptHash, pArgs, argCount);
}

void TestThread::DoRun()
{
	uint32_t playerPedId = NativeInvoke::Invoke<GET_PLAYER_PED, uint32_t>(-1);

	if(playerPedId != -1 && playerPedId != 0)
	{
		if(!m_bInitialized)
		{
			NativeInvoke::Invoke<LOAD_SCENE, int>(-786.44f, -48.50f, 37.75f);

			NativeInvoke::Invoke<SHUTDOWN_LOADING_SCREEN, int>();
			NativeInvoke::Invoke<DO_SCREEN_FADE_IN, int>(0);

			NativeInvoke::Invoke<SET_ENTITY_COORDS, int>(playerPedId, -786.44f, -48.50f, 37.75f);

			m_bInitialized = true;
		}

		static bool s_bWasF9Pressed = true;

		if(GetAsyncKeyState(VK_F9) & 0x8000)
		{
			if(!s_bWasF9Pressed)
			{
				NativeInvoke::Invoke<REQUEST_MODEL, int>(0xC1AE4D16);

				m_bIsWaitingForModelToLoad = true;
				s_bWasF9Pressed = true;
			}
		}
		else
		{
			s_bWasF9Pressed = false;
		}

		if(m_bIsWaitingForModelToLoad)
		{
			if(NativeInvoke::Invoke<HAS_MODEL_LOADED, bool>(0xC1AE4D16))
			{
				scrVector entityCoords = NativeInvoke::Invoke<GET_ENTITY_COORDS, scrVector>(playerPedId);

				NativeInvoke::Invoke<CREATE_VEHICLE, int>(0xC1AE4D16, entityCoords.x, entityCoords.y + 2, entityCoords.z, 0.0f, 1, 0);

				NativeInvoke::Invoke<SET_NOTIFICATION_TEXT_ENTRY, int>("STRING");
				NativeInvoke::Invoke<ADD_TEXT_COMPONENT_STRING, int>("Vehicle spawned.");
				NativeInvoke::Invoke<DRAW_NOTIFICATION, int>(FALSE, FALSE);

				m_bIsWaitingForModelToLoad = false;
			}
		}

		NativeInvoke::Invoke<SET_PED_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0);
		NativeInvoke::Invoke<SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0, 0.0);

		NativeInvoke::Invoke<SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0);
		NativeInvoke::Invoke<SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0);
		NativeInvoke::Invoke<SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME, int>(0.0);

		NativeInvoke::Invoke<SET_MAX_WANTED_LEVEL, int>(0);
	}
}
