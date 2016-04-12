#pragma once
#include <Networking/Networking.hxx>

class TestThread : public ScriptThread, public MessageReceiver
{
private:
	bool m_bInitialized;
	bool m_bIsWaitingForPedModelToLoad;
	bool m_bIsWaitingForVehicleModelToLoad;
	std::list<scrPed> m_spawnedPeds;
	bool m_bIsWaitingForClonePedModelToLoad;
	bool m_bClonePedSpawned;
	scrPed m_clonePed;

	V_Plus_NetworkClient connection;
	bool connected;

	void Handle(const ENetPeer* peer, PeerConnected& data) override;
	void Handle(const ENetPeer* peer, PeerDisconnected& data) override;
	void Handle(const ENetPeer* peer, ChatMessage& message) override;
	void Handle(const ENetPeer* peer, PlayerJoin& message) override;
	void Handle(const ENetPeer* peer, PlayerQuit& message) override;
	void Handle(const ENetPeer* peer, PlayerSpawn& message) override;
	void Handle(const ENetPeer* peer, PlayerDespawn& message) override;
	void Handle(const ENetPeer* peer, OnFootSync& message) override;

	void RunNetwork();

public:
	TestThread();

	virtual eScriptThreadState Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) override;
	virtual void DoRun() override;
};
