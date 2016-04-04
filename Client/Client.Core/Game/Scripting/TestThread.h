#pragma once

enum NativeIdentifiers : uint64_t
{
	GET_PLAYER_PED = 0x43A66C31C68491C0,
	GET_ENTITY_COORDS = 0x3FEF770D40960D5A,
	GET_FIRST_BLIP_INFO_ID = 0x1BEDE233E6CD2A1F,
	GET_NEXT_BLIP_INFO_ID = 0x14F96AA50D6FBEA7,
	GET_BLIP_INFO_ID_TYPE = 0xBE9B0959FFD0779B,
	GET_BLIP_COORDS = 0x586AFE3FF72D996E,
	GET_GROUND_Z_FOR_3D_COORD = 0xC906A7DAB05C8D2B,
	SET_ENTITY_COORDS = 0x621873ECE1178967,
	SET_ENTITY_COORDS_NO_OFFSET = 0x239A3351AC1DA385,
	LOAD_SCENE = 0x4448EB75B4904BDB,
	REQUEST_MODEL = 0x963D27A58DF860AC,
	HAS_MODEL_LOADED = 0x98A4EB5D89A0C952,
	CREATE_VEHICLE = 0xAF35D0D2583051B0,
	SHUTDOWN_LOADING_SCREEN = 0x078EBE9809CCD637,
	DO_SCREEN_FADE_IN = 0xD4E8E24955024033,
	NETWORK_IS_HOST = 0x8DB296B814EDDA07,
	NETWORK_RESURRECT_LOCAL_PLAYER = 0xEA23C49EAA83ACFB,
	NETWORK_IS_GAME_IN_PROGRESS = 0x10FAB35428CCC9D7,
	IS_ENTITY_DEAD = 0x5F9532F3B5CC2551,
	SET_NOTIFICATION_TEXT_ENTRY = 0x202709F4C58A0424,
	ADD_TEXT_COMPONENT_STRING = 0x6C188BE134E074AA,
	DRAW_NOTIFICATION = 0x2ED7843F8F801023,
	SET_PED_DENSITY_MULTIPLIER_THIS_FRAME = 0x95E3D6257B166CF2,
	SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME = 0x7A556143A1C03898,
	SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME = 0x245A6883D966D537,
	SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME = 0xEAE6DCC7EEE3DB1D,
	SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME = 0xB3B3359379FE77D3,
	SET_MAX_WANTED_LEVEL = 0xAA5F02DB48D704B9
};

class TestThread : public ScriptThread
{
private:
	bool m_bInitialized;
	bool m_bIsWaitingForModelToLoad;

public:
	TestThread();

	virtual eScriptThreadState Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) override;
	virtual void DoRun() override;
};